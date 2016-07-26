/*
Module : APIPE.CPP
Purpose: Defines the implementation for an MFC wrapper class
         for Win32 anonymous Pipes
*/


/////////////////////////////////  Includes  //////////////////////////////////
#include "stdafx.h"
#include "apipe.h"
#include <Tlhelp32.h>// DWORD GetProcessIdFromName(LPCTSTR name) 2012.08.16 yyf
 

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

// 匿名管道线程 不停的读信息.
DWORD WINAPI  AnonymousPipeThread(LPVOID param)
{
	CAnonymousPipe * pAnPipe = (CAnonymousPipe *)param;

	BYTE BYTE_Read[10240] ;
	while(NULL != pAnPipe && pAnPipe->bAnonymousPipeThread)
	{
		DWORD dwRead;

		DWORD  dwBytesRead;
		DWORD  dwTotalBytesAvail;
		DWORD dwBytesLeftThisMessage; 

		if (pAnPipe->Peek((LPVOID)BYTE_Read,10240,  dwBytesRead, dwTotalBytesAvail, dwBytesLeftThisMessage))
		{
			if(pAnPipe->Read((LPVOID)BYTE_Read,10240,dwRead))
			{
				pAnPipe->ReadAnonymousPipe((LPVOID)BYTE_Read,dwRead);
			} 
			else
			{
				OutputDebugString("yyf: 匿名管道线程.读失败.");
				Sleep(500);
			}
		}
		else
		{ 
			Sleep(500);
		}
	}
	OutputDebugString("yyf: 匿名管道线程结束.");
	return 0;
}
///////////////////////////// CAnonymousPipe Implementation ///////////////////

IMPLEMENT_DYNAMIC(CAnonymousPipe, CObject)

CAnonymousPipe::CAnonymousPipe()
{
  m_hWrite = INVALID_HANDLE_VALUE;//实际操作的写句柄
  m_hRead = INVALID_HANDLE_VALUE;//实际操作的读句柄
 
  m_hFatherWrite = INVALID_HANDLE_VALUE;//父进程写句柄
  m_hFatherRead = INVALID_HANDLE_VALUE;//父进程读句柄
  m_hChildWrite = INVALID_HANDLE_VALUE;//子进程写句柄
  m_hChildRead = INVALID_HANDLE_VALUE;//子进程读句柄

  bAnonymousPipeThread = false;//控制线程开工吧
}

CAnonymousPipe::~CAnonymousPipe()
{
  Close();
}

BOOL CAnonymousPipe::Create(LPSECURITY_ATTRIBUTES lpPipeAttributes1 , DWORD dwSize1 ,LPSECURITY_ATTRIBUTES lpPipeAttributes2 , DWORD dwSize2 )
{
  ASSERT(!IsOpen());

  //管道1: 父进程写->子进程读 
  BOOL bSuccess1 = ::CreatePipe(&m_hChildRead, &m_hFatherWrite, lpPipeAttributes1, dwSize1);
  if (!bSuccess1)
  {
    TRACE1("CAnonymousPipe::Create() failed, GetLastError returned %d\n", ::GetLastError());
  }

  //管道2: 子进程写->父进程读 
  BOOL bSuccess2 = ::CreatePipe(&m_hFatherRead, &m_hChildWrite, lpPipeAttributes2, dwSize2);
  if (!bSuccess2)
  {
	  TRACE1("CAnonymousPipe::Create() failed, GetLastError returned %d\n", ::GetLastError());
  }
  
  if (bSuccess1 && bSuccess2)
  {//本进程是父进程,所以都用的是父句柄
	   m_hRead = m_hFatherRead;  //实际操作的读句柄 
	   m_hWrite= m_hFatherWrite;//实际操作的写句柄
	   return true;
  }
  return false;
}

BOOL CAnonymousPipe::Write(LPCVOID lpBuffer, DWORD dwNumberOfBytesToWrite, DWORD& dwNumberOfBytesWritten)
{
  ASSERT(m_hWrite != NULL); //Pipe must be open
  
  if(NULL == m_hWrite)
  {
	  return false;
  }
  BOOL bSuccess = ::WriteFile(m_hWrite, lpBuffer, dwNumberOfBytesToWrite, 
                              &dwNumberOfBytesWritten, NULL);
  if (!bSuccess)
    TRACE1("CAnonymousPipe::Write() failed, GetLastError returned %d\n", ::GetLastError());

  return bSuccess;
}

//管道
BOOL CAnonymousPipe::Read(LPVOID lpBuffer, DWORD dwNumberOfBytesToRead, DWORD& dwNumberOfBytesRead)
{	
  ASSERT(m_hRead != NULL); //Pipe must be open
  if(NULL == m_hRead)
  {
	  return false;
  }

  BOOL bSuccess = ::ReadFile(m_hRead, lpBuffer, dwNumberOfBytesToRead, 
                             &dwNumberOfBytesRead, NULL);
  if (!bSuccess)
    TRACE1("CAnonymousPipe::Read() failed, GetLastError returned %d\n", ::GetLastError());

  return bSuccess;
}

void CAnonymousPipe::ReadAnonymousPipe(LPVOID lpBuffer, DWORD dwNumberOfBytesToRead)
{ 
	if(NULL != piAnonymousPipe )
	{//调用回调
		piAnonymousPipe->ReadAnonymousPipe(lpBuffer, dwNumberOfBytesToRead);
	}  
}

BOOL CAnonymousPipe::Peek(LPVOID lpBuffer, DWORD dwBufferSize, DWORD& dwBytesRead, 
                          DWORD& dwTotalBytesAvail, DWORD& dwBytesLeftThisMessage)
{
  ASSERT(m_hRead != NULL);

  if(NULL == m_hRead)
  {
	  return false;
  }

  BOOL bSuccess = ::PeekNamedPipe(m_hRead, lpBuffer, dwBufferSize, &dwBytesRead, 
                                  &dwTotalBytesAvail, &dwBytesLeftThisMessage);
  if (!bSuccess)
    TRACE1("CNamedPipe::Peek() failed, GetLastError returned %d\n", ::GetLastError());

  return bSuccess;
}

BOOL CAnonymousPipe::Close()
{
	
  bAnonymousPipeThread = false;

  BOOL bSuccess = TRUE;
  if (IsOpen())
  { 
	  BOOL bSuccess1 = TRUE;
	  BOOL bSuccess2 = TRUE;
	  BOOL bSuccess3 = TRUE;
	  BOOL bSuccess4 = TRUE;
	  BOOL bSuccess5 = TRUE;
	  BOOL bSuccess6 = TRUE; 

	if (m_hChildRead != INVALID_HANDLE_VALUE)
	{
		bSuccess1 = CloseHandle(m_hChildRead);
	}
	if (m_hChildWrite != INVALID_HANDLE_VALUE)
	{
		bSuccess2 = CloseHandle(m_hChildWrite);
	}
	if (m_hFatherRead != INVALID_HANDLE_VALUE)
	{
		bSuccess3 = CloseHandle(m_hFatherRead);
	}
	if (m_hFatherWrite != INVALID_HANDLE_VALUE)
	{
		bSuccess4 = CloseHandle(m_hFatherWrite); 
	}
     
	bSuccess = (bSuccess1&bSuccess2&bSuccess3&bSuccess4);

	if (m_hFatherWrite == INVALID_HANDLE_VALUE && //父进程写句柄
		m_hFatherRead == INVALID_HANDLE_VALUE && //父进程读句柄
		m_hChildWrite == INVALID_HANDLE_VALUE&& //子进程写句柄
		m_hChildRead == INVALID_HANDLE_VALUE//子进程读句柄
		)
	{//说明此时是 子进程的关闭.
		bSuccess5 = CloseHandle(m_hWrite);
		if (!bSuccess5)
		{
			TRACE1("CAnonymousPipe::Close() failed, GetLastError returned %d\n", GetLastError());
		}

		bSuccess6 = CloseHandle(m_hRead);
		if (!bSuccess6)
		{
			TRACE1("CAnonymousPipe::Close() failed, GetLastError returned %d\n", GetLastError());
		}

		bSuccess = (bSuccess5 & bSuccess6);
	}

	m_hWrite = INVALID_HANDLE_VALUE;//实际操作的写句柄
	m_hRead = INVALID_HANDLE_VALUE;//实际操作的读句柄

	m_hFatherWrite = INVALID_HANDLE_VALUE;//父进程写句柄
	m_hFatherRead = INVALID_HANDLE_VALUE;//父进程读句柄
	m_hChildWrite = INVALID_HANDLE_VALUE;//子进程写句柄
	m_hChildRead = INVALID_HANDLE_VALUE;//子进程读句柄
  }

  return bSuccess;
}

#ifdef _DEBUG
void CAnonymousPipe::AssertValid() const
{
  CObject::AssertValid();
  ASSERT(IsOpen());
}
#endif

#ifdef _DEBUG
void CAnonymousPipe::Dump(CDumpContext& dc) const
{
  CObject::Dump(dc);

  CString sText;
  sText.Format(_T("Open=%d, Write Handle=%x, Read Handle=%x\n"), IsOpen(), m_hWrite, m_hRead);
  dc << sText;
}
#endif



void CAnonymousPipe::SetAnonymousPipe(IAnonymousPipe * iAnonymousPipe)
{ 
	 this->piAnonymousPipe = iAnonymousPipe; 
}

//2012.08.16 yyf 返回进程的PID
DWORD CAnonymousPipe::GetProcessIdFromName(LPCTSTR name) //如果有运行，返回进程的PID，没运行返回0 //自己可以修改函数，返回运行中的进程的个数
{ 
	PROCESSENTRY32 pe; 
	DWORD id = 0; 
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0); 
	pe.dwSize = sizeof(PROCESSENTRY32); 
	if( !Process32First(hSnapshot,&pe) ) 
	{
		return 0; 
	}

	while(1) 
	{ 
		pe.dwSize = sizeof(PROCESSENTRY32); 
		if( Process32Next(hSnapshot,&pe)==FALSE ) 
			break; 
		//if(strcmp(pe.szExeFile,name) == 0) 
		//{ 
		//	id = pe.th32ProcessID; 
		//	break; 
		//} 
		if(_stricmp(pe.szExeFile,name) == 0) //忽略大小写字母比较
		{ 
			id = pe.th32ProcessID; 
			break; 
		} 

	}
	CloseHandle(hSnapshot); 
	return id; 
} 


// //打开一个进程 2012.08.17 yyf
bool CAnonymousPipe::OpenProcess(LPCSTR lpApplicationName)
{
	strProcessName = lpApplicationName;//进程名字
	if(0 == GetProcessIdFromName(strProcessName) && NULL != piAnonymousPipe)
	{//进程没开始

		m_hWrite = INVALID_HANDLE_VALUE;
		m_hRead = INVALID_HANDLE_VALUE;

		m_hFatherWrite = INVALID_HANDLE_VALUE;//父进程写句柄
		m_hFatherRead = INVALID_HANDLE_VALUE;//父进程读句柄
		m_hChildWrite = INVALID_HANDLE_VALUE;//子进程写句柄
		m_hChildRead = INVALID_HANDLE_VALUE;//子进程读句柄

		bAnonymousPipeThread = false;//控制线程开工吧

		//创建2个匿名管道，一个用于父进程写->子进程读，一个用于子进程写->父进程读.
		
		//管道1: 父进程写->子进程读
		SECURITY_ATTRIBUTES sa1;
		sa1.bInheritHandle=TRUE;
		sa1.lpSecurityDescriptor=NULL;
		sa1.nLength=sizeof(SECURITY_ATTRIBUTES);

		//管道2: 子进程写->父进程读
		SECURITY_ATTRIBUTES sa2;
		sa2.bInheritHandle=TRUE;
		sa2.lpSecurityDescriptor=NULL;
		sa2.nLength=sizeof(SECURITY_ATTRIBUTES);

		if(!Create(&sa1,0,&sa2,0))
		{
			// AfxMessageBox("创建匿名管道失败！");
			return false;
		}

		STARTUPINFO sui;
		//PROCESS_INFORMATION pi;
		ZeroMemory(&sui,sizeof(STARTUPINFO));
		sui.cb=sizeof(STARTUPINFO);
		sui.dwFlags=STARTF_USESTDHANDLES;
		sui.hStdInput=m_hChildRead;//子进程写句柄
		sui.hStdOutput=m_hChildWrite;//子进程读句柄
		sui.hStdError=GetStdHandle(STD_ERROR_HANDLE);
	
		if(!CreateProcess(strProcessName,NULL,NULL,NULL,
				TRUE,0,NULL,NULL,&sui,&pi))//"..\\Child\\Debug\\Child.exe"
		{
			CloseHandle(m_hRead);
			CloseHandle(m_hWrite);
			m_hRead=INVALID_HANDLE_VALUE;
			m_hWrite=INVALID_HANDLE_VALUE;

			CloseHandle(m_hChildRead);
			CloseHandle(m_hChildWrite);
			m_hFatherWrite = INVALID_HANDLE_VALUE;//父进程写句柄
			m_hFatherRead = INVALID_HANDLE_VALUE;//父进程读句柄
			m_hChildWrite = INVALID_HANDLE_VALUE;//子进程写句柄
			m_hChildRead = INVALID_HANDLE_VALUE;//子进程读句柄
			//MessageBox("创建子进程失败！");
			return false;
		}
		//else
		//{
		//	CloseHandle(pi.hProcess);
		//	CloseHandle(pi.hThread);
		//}

		//创建匿名管道线程，开始读
		bAnonymousPipeThread = true; 
		DWORD dwThreadID = 0;
		HANDLE hThread = CreateThread(0,0,AnonymousPipeThread,this,0,&dwThreadID);
		if (hThread)
		{
			OutputDebugString("yyf: 匿名管道线程开始.");
			::CloseHandle(hThread);
		} 
		 
		return true;
	}
	return false;
}

// //设置子进程 读写句柄 
void CAnonymousPipe::SetChildPipe(void)
{ 
	m_hRead=GetStdHandle(STD_INPUT_HANDLE);
	m_hWrite=GetStdHandle(STD_OUTPUT_HANDLE);
}




//开始匿名管道线程
void CAnonymousPipe::StartThread(void)
{ 
		//创建匿名管道线程，开始读
		bAnonymousPipeThread = true; 
		DWORD dwThreadID = 0;
		HANDLE hThread = CreateThread(0,0,AnonymousPipeThread,this,0,&dwThreadID);
		if (hThread)
		{			
			::CloseHandle(hThread);
		}
		else
		{
			OutputDebugString("yyf: update.exe 匿名管道线程失败.");
		}
}

// 初始化父进程管道
void CAnonymousPipe::InitFatherPipe(IAnonymousPipe *  iAnonymousPipe)
{
	SetAnonymousPipe(iAnonymousPipe);//初始化 匿名管道  
}

// 初始化子进程管道
void CAnonymousPipe::InitChildPipe(IAnonymousPipe * iAnonymousPipe)
{
	 SetAnonymousPipe(iAnonymousPipe);//初始化 匿名管道 
	 SetChildPipe();
	 StartThread();
}
