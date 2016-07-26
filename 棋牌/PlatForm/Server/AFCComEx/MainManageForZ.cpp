#include "StdAfx.h"
#include "MainManageForZ.h"
#include "AFCException.h"
#include "GameRoomMessage.h"

#pragma warning (disable: 4355)

//处理类型定义
#define HD_SOCKET_READ					1							//SOCKET 读取事件处理			
#define HD_SOCKET_CLOSE					2							//SOCKET 关闭事件处理			
#define HD_DATA_BASE_RESULT				3							//数据库结果处理
#define HD_TIMER_MESSAGE				4							//定时器消息处理
extern CString GetAppPath(bool bFource=true); 
extern void DebugPrintf(const char *p, ...);

//窗口线程启动结构
struct WindowThreadStartStruct
{
	//变量定义
	HANDLE								hEvent;						//启动事件
	BOOL								bSuccess;					//启动成功标志
	CBaseMainManageForZ						* pMainManage;				//数据管理指针
};

//处理线程启动结构
struct HandleThreadStartStruct
{
	//变量定义
	HANDLE								hEvent;						//启动事件
	HANDLE								hCompletionPort;			//完成端口
	CBaseMainManageForZ						* pMainManage;				//数据管理指针
};

//SOCKET 关闭通知结构定义
struct SocketCloseLine
{
	DataLineHead						LineHead;					//队列头
	UINT								uIndex;						//SOCKT 索引
	ULONG								uAccessIP;					//SOCKET IP
	long int							lConnectTime;				//连接时间
};

//SOCKET 读取通知结构定义
struct SocketReadLine
{
	DataLineHead						LineHead;					//队列头
	NetMessageHead						NetMessageHead;				//数据包头
	UINT								uHandleSize;				//数据包处理大小
	UINT								uIndex;						//SOCKET 索引
	ULONG								uAccessIP;					//SOCKET IP
	DWORD								dwHandleID;					//SOCKET 处理 ID
};

//定时器消息结构定义
struct WindowTimerLine
{
	DataLineHead						LineHead;					//队列头
	UINT								uTimerID;					//定时器 ID
};

/*****************************************************************************************************************/

//构造函数
CBaseMainManageForZ::CBaseMainManageForZ(void)
{
	m_bInit=false;
	m_bRun=false;
	m_hWindow=NULL;
	m_hHandleThread=NULL;
	m_hWindowThread=NULL;
	m_hCompletePort=NULL;
	::memset(&m_DllInfo,0,sizeof(m_DllInfo));
	::memset(&m_InitData,0,sizeof(m_InitData));
	::memset(&m_KernelData,0,sizeof(m_KernelData));
}

//析构函数	
CBaseMainManageForZ::~CBaseMainManageForZ(void)
{
}

//初始化函数 
bool CBaseMainManageForZ::Init(ManageInfoStruct * pInitData, IDataBaseHandleService * pDataHandleService)
{
	if ((this==NULL)||(m_bInit==true)) return false;

	//设置数据
	UINT uMax=pInitData->uMaxPeople;
	m_InitData=*pInitData;
	if (!PreInitParameter(&m_InitData,&m_KernelData)) throw new CAFCException(TEXT("CBaseMainManageForZ::Init PreInitParameter 参数调节错误"),0x41A);
	if(m_InitData.uMaxPeople<uMax)
		m_InitData.uMaxPeople=uMax;

	//初始化组件
	if (m_KernelData.bStartTCPSocket) 
		m_TCPSocket.Init(m_InitData.uMaxPeople,m_InitData.uListenPort,m_KernelData.bMaxVer,m_KernelData.bLessVer,m_InitData.iSocketSecretKey,this);
	if (m_KernelData.bStartSQLDataBase)
	{
		if (pDataHandleService!=NULL)
			pDataHandleService->SetParameter(this,&m_SQLDataManage,&m_InitData,&m_KernelData);
		m_SQLDataManage.Init(&m_InitData,&m_KernelData,pDataHandleService,this);
	}

	//调用接口
	if (OnInit(&m_InitData,&m_KernelData)==false)  
		throw new CAFCException(TEXT("CBaseMainManageForZ::Init OnInit 函数错误"),0x41B);

	m_bInit=true;
	CString s = GetAppPath();
	m_TalkFilter.LoadFilterMessage(s.GetBuffer());
	return true;
}

//取消初始化函数 
bool CBaseMainManageForZ::UnInit()
{
	if (this==NULL) return false;
	//////////////////////////////////
	//Kylin 20090209 清除在线用户在线标记

	//停止服务
	if (m_bRun) Stop();

	//调用接口
	OnUnInit();
	//////////////////////////////////
	//取消初始化
	m_bInit=false;
	m_TCPSocket.UnInit();
	m_SQLDataManage.UnInit();

	//设置数据
	memset(&m_DllInfo,0,sizeof(m_DllInfo));
	memset(&m_InitData,0,sizeof(m_InitData));
	memset(&m_KernelData,0,sizeof(m_KernelData));

	return true;
}

//启动函数
bool CBaseMainManageForZ::Start()
{	
	if ((this==NULL)||(m_bRun==true)||(m_bInit==false)) return false;
	//建立事件
	CEvent StartEvent(FALSE,TRUE,NULL,NULL);
	if (StartEvent==NULL) throw new CAFCException(TEXT("CBaseMainManageForZ::Start 事件建立失败"),0x41C);
	//建立完成端口
	m_hCompletePort=::CreateIoCompletionPort(INVALID_HANDLE_VALUE,NULL,NULL,0);
	if (m_hCompletePort==NULL) throw new CAFCException(TEXT("CBaseMainManageForZ::Start m_hCompletePort 建立失败"),0x41D);
	m_DataLine.SetCompletionHandle(m_hCompletePort);
	
	//启动处理线程
	UINT uThreadID=0;
	HandleThreadStartStruct	ThreadStartData;
	ThreadStartData.pMainManage=this;
	ThreadStartData.hCompletionPort=m_hCompletePort;
	ThreadStartData.hEvent=StartEvent;
	m_hHandleThread=(HANDLE)_beginthreadex(NULL,0,LineDataHandleThread,&ThreadStartData,0,&uThreadID);
	if (m_hHandleThread==NULL) throw new CAFCException(TEXT("CBaseMainManageForZ::Start LineDataHandleThread 线程启动失败"),0x41E);
	WaitForSingleObject(StartEvent,INFINITE);
	
	//启动组件
	AFCKernelStart();
	
	if (m_KernelData.bStartSQLDataBase)	m_SQLDataManage.Start();
	
	if (m_KernelData.bStartTCPSocket) m_TCPSocket.Start(m_KernelData.uAcceptThreadCount,m_KernelData.uSocketThreadCount);
	
	//调用接口
	if (OnStart()==false) throw new CAFCException(TEXT("CBaseMainManageForZ::Start OnStart 函数错误"),0x41F);
	
	//设置数据
	m_bRun=true;
	return true;
}

//停止服务
bool CBaseMainManageForZ::Stop()
{
	if (this==NULL) return false;

	//调用接口
	OnStop();

	//停止组件
	m_bRun=false;
	m_DataLine.SetCompletionHandle(NULL);

	m_TCPSocket.Stop();
	m_SQLDataManage.Stop();

	//关闭窗口
	if ((m_hWindow!=NULL)&&(::IsWindow(m_hWindow)==TRUE)) ::SendMessage(m_hWindow,WM_CLOSE,0,0);

	//关闭完成端口
	if (m_hCompletePort!=NULL)
	{
		::PostQueuedCompletionStatus(m_hCompletePort,0,NULL,NULL);
		::CloseHandle(m_hCompletePort);
		m_hCompletePort=NULL;
	}

	//退出窗口线程
	if ((m_hWindowThread!=NULL)&&(::WaitForSingleObject(m_hWindowThread,3000)==WAIT_TIMEOUT))
	{
		TerminateThread(m_hWindowThread,0);	
		CloseHandle(m_hWindowThread);
		m_hWindowThread=NULL;
	}

	//退出处理线程
	if ((m_hHandleThread!=NULL)&&(::WaitForSingleObject(m_hHandleThread,3000)==WAIT_TIMEOUT))
	{
		TerminateThread(m_hHandleThread,0);
		CloseHandle(m_hHandleThread);
		m_hHandleThread=NULL;
	}

	return true;
}

//网络关闭处理
bool CBaseMainManageForZ::OnSocketCloseEvent(ULONG uAccessIP, UINT uIndex, long int lConnectTime)
{
	SocketCloseLine SocketClose;
	SocketClose.lConnectTime=lConnectTime;
	SocketClose.uIndex=uIndex;
	SocketClose.uAccessIP=uAccessIP;
	return (m_DataLine.AddData(&SocketClose.LineHead,sizeof(SocketClose),HD_SOCKET_CLOSE)!=0);
}

//网络消息处理
bool CBaseMainManageForZ::OnSocketReadEvent(CTCPSocketForZ * pSocket, NetMessageHead * pNetHead, void * pData, UINT uSize, UINT uIndex, DWORD dwHandleID)
{
	SocketReadLine SocketRead;
	SocketRead.uHandleSize=uSize;
	SocketRead.uIndex=uIndex;
	SocketRead.dwHandleID=dwHandleID;
	SocketRead.uAccessIP=pSocket->GetConnectData();
	SocketRead.NetMessageHead=*pNetHead;
	return (m_DataLine.AddData(&SocketRead.LineHead,sizeof(SocketRead),HD_SOCKET_READ,pData,uSize)!=0);
}

//定时器通知消息
bool CBaseMainManageForZ::WindowTimerMessage(UINT uTimerID)
{
	WindowTimerLine WindowTimer;
	WindowTimer.uTimerID=uTimerID;
	return (m_DataLine.AddData(&WindowTimer.LineHead,sizeof(WindowTimer),HD_TIMER_MESSAGE)!=0);
}

//数据库结果处理
bool CBaseMainManageForZ::OnDataBaseResultEvent(DataBaseResultLine * pResultData, UINT uHandleKind, UINT uHandleResult, UINT uResultSize, UINT uIndex, DWORD dwHandleID)
{
	//包装数据
	pResultData->LineHead.uSize=uResultSize;
	pResultData->uHandleKind=uHandleKind;
	pResultData->uHandleRusult=uHandleResult;
	pResultData->dwHandleID=dwHandleID;
	pResultData->uIndex=uIndex;

	//加入队列
	return (m_DataLine.AddData(&pResultData->LineHead,uResultSize,HD_DATA_BASE_RESULT)!=0);
}

//设定定时器
bool CBaseMainManageForZ::SetTimer(UINT uTimerID, UINT uElapse)
{
	if ((m_hWindow!=NULL)&&(IsWindow(m_hWindow)==TRUE))
	{
		::SetTimer(m_hWindow,uTimerID,uElapse,NULL);
		return true;
	}
	return false;
}

//清除定时器
bool CBaseMainManageForZ::KillTimer(UINT uTimerID)
{
	if ((m_hWindow!=NULL)&&(::IsWindow(m_hWindow)==TRUE))
	{
		::KillTimer(m_hWindow,uTimerID);
		return true;
	}
	return false;
}

//内核初始化函数
bool CBaseMainManageForZ::AFCKernelStart()
{
	if ((m_hWindow!=NULL)&&(::IsWindow(m_hWindow)==TRUE)) return false;

	//建立事件
	CEvent StartEvent(FALSE,TRUE,NULL,NULL);
	if (StartEvent==NULL) throw new CAFCException(TEXT("CBaseMainManageForZ::AFCKernelStart 事件建立失败"),0x420);

	//建立线程
	WindowThreadStartStruct ThreadData;
	ThreadData.bSuccess=FALSE;
	ThreadData.pMainManage=this;
	ThreadData.hEvent=StartEvent;

	UINT uThreadID=0;
	m_hWindowThread=(HANDLE)::_beginthreadex(NULL,0,WindowMsgThread,&ThreadData,0,&uThreadID);
	if (m_hWindowThread==NULL) throw new CAFCException(TEXT("CBaseMainManageForZ::AFCKernelStart WindowMsgThread 线程建立失败"),0x421);
	::WaitForSingleObject(ThreadData.hEvent,INFINITE);
	if (ThreadData.bSuccess==FALSE) throw new CAFCException(TEXT("CBaseMainManageForZ::AFCKernelStart WindowMsgThread 线程建立失败"),0x422);

	return true;
}

//队列数据处理线程
unsigned __stdcall CBaseMainManageForZ::LineDataHandleThread(LPVOID pThreadData)
{
	//数据定义
	HandleThreadStartStruct		* pData=(HandleThreadStartStruct *)pThreadData;		//线程启动数据指针
	CBaseMainManageForZ				* pMainManage=pData->pMainManage;					//数据管理指针
	CDataLine					* m_pDataLine=&pMainManage->m_DataLine;				//数据队列指针
	HANDLE						hCompletionPort=pData->hCompletionPort;				//完成端口
	bool						& bRun=pMainManage->m_bRun;							//运行标志

	//线程数据读取完成
	::SetEvent(pData->hEvent);

	//重叠数据
	void						* pIOData=NULL;										//数据
	DWORD						dwThancferred=0;									//接收数量
	ULONG						dwCompleteKey=0L;									//重叠 IO 临时数据
	LPOVERLAPPED				OverData;											//重叠 IO 临时数据

	//数据缓存
	BOOL						bSuccess=FALSE;
	BYTE						szBuffer[LD_MAX_PART];
	DataLineHead				* pDataLineHead=(DataLineHead *)szBuffer;

	while (1)
	{
		//等待完成端口
		bSuccess=::GetQueuedCompletionStatus(hCompletionPort,&dwThancferred,&dwCompleteKey,(LPOVERLAPPED *)&OverData,INFINITE);
		if ((bSuccess==FALSE)||dwThancferred==0) ::_endthreadex(0);	
		//处理数据
		while(m_pDataLine->GetDataCount())//当队列中处理信息过多,会不会占用太多的cpu时间而使其他线程延时严重
		{
			//处理数据
			try
			{
				//处理完成端口数据
				if ((bRun==false)||(m_pDataLine->GetData(pDataLineHead,sizeof(szBuffer))==0)) continue;

				switch (pDataLineHead->uDataKind)
				{
				case HD_SOCKET_READ:		//SOCKET 数据读取
					{
						SocketReadLine * pSocketRead=(SocketReadLine *)pDataLineHead;
						try
						{
							bool bSuccess=pMainManage->OnSocketRead(&pSocketRead->NetMessageHead,
								pSocketRead->uHandleSize?pSocketRead+1:NULL,
								pSocketRead->uHandleSize,pSocketRead->uAccessIP,
								pSocketRead->uIndex,pSocketRead->dwHandleID);
							if (bSuccess==false) 
							{
								TRACE("HERE IS FAILED!\n");
								throw ("网络数据包处理出现异常");
							}//
						}
						catch (...)
						{
							TRACE("CATCH:%s with %s\n",__FILE__,__FUNCTION__);
							//DebugPrintf("CATCH:%s with %s\n",__FILE__,__FUNCTION__);
							pMainManage->m_TCPSocket.CloseSocket(pSocketRead->uIndex,pSocketRead->dwHandleID);
						}
						break;
					}
				case HD_SOCKET_CLOSE:		//SOCKET 关闭
					{
						SocketCloseLine * pSocketClose=(SocketCloseLine *)pDataLineHead;
						pMainManage->OnSocketClose(pSocketClose->uAccessIP,pSocketClose->uIndex,pSocketClose->lConnectTime);
						break;
					}
				case HD_DATA_BASE_RESULT:	//数据库处理结果
					{
						DataBaseResultLine * pDataResult=(DataBaseResultLine *)pDataLineHead;
						pMainManage->OnDataBaseResult(pDataResult);
						break;
					}
				case HD_TIMER_MESSAGE:		//定时器消息
					{
						WindowTimerLine * pTimerMessage=(WindowTimerLine *)pDataLineHead;
						pMainManage->OnTimerMessage(pTimerMessage->uTimerID);
						break;
					}
				}
			}

			catch (...) 
			{ TRACE("CATCH:%s with %s %d\n",__FILE__,__FUNCTION__,__LINE__);
			continue;
			}
		}
	}

	::_endthreadex(0);
	return 0;
}

//WINDOW 消息循环线程
unsigned __stdcall CBaseMainManageForZ::WindowMsgThread(LPVOID pThreadData)
{
	WindowThreadStartStruct * pStartData=(WindowThreadStartStruct *)pThreadData;

	try
	{
		//注册窗口类
		LOGBRUSH		LogBrush;
		WNDCLASS		WndClass;
		TCHAR			szClassName[]=TEXT("CMainManageWindow");

		LogBrush.lbColor=RGB(0,0,0);
		LogBrush.lbStyle=BS_SOLID;
		LogBrush.lbHatch=0;
		WndClass.cbClsExtra=0;
		WndClass.cbWndExtra=0;
		WndClass.hCursor=NULL;
		WndClass.hIcon=NULL;
		WndClass.lpszMenuName=NULL;
		WndClass.lpfnWndProc=WindowProcFunc;
		WndClass.lpszClassName=szClassName;
		WndClass.style=CS_HREDRAW|CS_VREDRAW;
		WndClass.hInstance=NULL;
		WndClass.hbrBackground=(HBRUSH)::CreateBrushIndirect(&LogBrush);
		::RegisterClass(&WndClass);

		//建立窗口
		pStartData->pMainManage->m_hWindow=::CreateWindow(szClassName,NULL,0,0,0,0,0,NULL,NULL,NULL,pStartData->pMainManage);
		if (pStartData->pMainManage->m_hWindow==NULL) throw TEXT("窗口建立失败");
	}
	catch (...)
	{TRACE("CATCH:%s with %s\n",__FILE__,__FUNCTION__);
	//启动错误
	pStartData->bSuccess=FALSE;
	::SetEvent(pStartData->hEvent);
	_endthreadex(0);
	}

	//启动成功
	pStartData->bSuccess=TRUE;
	::SetEvent(pStartData->hEvent);

	//消息循环
	MSG	Message;
	while (::GetMessage(&Message,NULL,0,0))
	{
		if (!::TranslateAccelerator(Message.hwnd,NULL,&Message))
		{
			::TranslateMessage(&Message);
			::DispatchMessage(&Message);
		}
	}

	_endthreadex(0);
	return 0;
}

//窗口回调函数
LRESULT CALLBACK CBaseMainManageForZ::WindowProcFunc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CREATE:		//窗口建立消息
		{
			DWORD iIndex=TlsAlloc();
			CBaseMainManageForZ * pMainManage=(CBaseMainManageForZ *)(((CREATESTRUCT *)lParam)->lpCreateParams);
			TlsSetValue(iIndex,pMainManage);
			::SetWindowLong(hWnd,GWL_USERDATA,iIndex);
			break;
		}
	case WM_TIMER:		//定时器消息
		{
			DWORD iIndex=::GetWindowLong(hWnd,GWL_USERDATA);
			CBaseMainManageForZ * pMainManage=(CBaseMainManageForZ *)::TlsGetValue(iIndex);
			if ((pMainManage!=NULL)&&(pMainManage->WindowTimerMessage((UINT)wParam)==false)) ::KillTimer(hWnd,(UINT)wParam);
			break;
		}
	case WM_CLOSE:		//窗口关闭消息
		{
			DestroyWindow(hWnd);
			break;
		}
	case WM_DESTROY:	//窗口关闭消息
		{
			DWORD iIndex=::GetWindowLong(hWnd,GWL_USERDATA);
			CBaseMainManageForZ * pMainManage=(CBaseMainManageForZ *)::TlsGetValue(iIndex);
			if (pMainManage!=NULL) pMainManage->m_hWindow=NULL;
			::TlsFree(iIndex);
			PostQuitMessage(0);
			break;
		}
	}
	return ::DefWindowProc(hWnd,uMsg,wParam,lParam);
}
