#include "StdAfx.h"
#include "Math.h"
#include "AFCDataBase.h"
#include "AFCFunction.h"
#include "AFCSocket.h"
#include "OleDBErr.h"

extern void DebugPrintf(const char *p, ...);
//***********************************************************************************************//
void DebugPrintf(const char *p, ...)
{
	char szFilename[256];
	sprintf( szFilename, "DebugInfo\\PlatformServer.txt");
	FILE *fp = fopen( szFilename, "a" );
	if (NULL == fp)
	{
		return;
	}
	va_list arg;
	va_start( arg, p );
	fprintf(fp,"[%s]--", CTime::GetCurrentTime().Format("%H:%M:%S"));
	vfprintf(fp, p, arg );
	fprintf(fp,"\n");
	fclose(fp);
}

//处理线程结构定义
struct ThreadStartStruct
{
	//变量定义
	HANDLE						hEvent;									//退出事件
	HANDLE						hCompletionPort;						//完成端口
	CAFCDataBaseManage			* pDataManage;							//数据库管理类指针
};

//构造函数
CAFCDataBaseManage::CAFCDataBaseManage()
{
	m_bInit=false;
	m_bRun=false;
	m_hThread=NULL;
	m_hCompletePort=NULL;
	m_pInitInfo=NULL;
	m_pKernelInfo=NULL;
	m_pHandleService=NULL;

	hDBLogon=NULL;
	hDBNative=NULL;

	m_sqlClass = 0;
	m_nPort = 0;
	m_bsqlInit = FALSE;

	m_pConnection.CreateInstance(__uuidof(Connection));
	m_pRecordset.CreateInstance(_uuidof(Recordset));//初始化Recordset指针
	
}

//析构函数
CAFCDataBaseManage::~CAFCDataBaseManage(void)
{
}

//开始服务
bool CAFCDataBaseManage::Start()
{
	if ((this==NULL)||(m_bRun==true)||(m_bInit==false)) throw new CAFCException(TEXT("CAFCDataBaseManage::Start 没有初始化参数"),0x401);
	
	//建立事件
	CEvent StartEvent(FALSE,TRUE,NULL,NULL);
	if (StartEvent==NULL) throw new CAFCException(TEXT("CAFCDataBaseManage::Start 事件建立失败"),0x402);

	//建立完成端口
	m_hCompletePort=CreateIoCompletionPort(INVALID_HANDLE_VALUE,NULL,NULL,0);
	if (m_hCompletePort==NULL) 
		throw new CAFCException(TEXT("CAFCDataBaseManage::Start m_hCompletePort 建立失败"),0x403);
	m_DataLine.SetCompletionHandle(m_hCompletePort);
		
	//通过 sqlengine 连接数据库	
	SQLConnectReset();
	SQLConnect();

	int x=0;
	//建立数据处理线程
	unsigned dwThreadID=0;
	ThreadStartStruct ThreadStartData;
	ThreadStartData.pDataManage=this;
	ThreadStartData.hEvent=StartEvent;
	ThreadStartData.hCompletionPort=m_hCompletePort;
	m_hThread=(HANDLE)_beginthreadex(NULL,0,DataServiceThread,&ThreadStartData,0,&dwThreadID);
	if (m_hThread==NULL) throw new CAFCException(TEXT("CAFCDataBaseManage::Start DataServerThread 线程建立失败"),0x407);
	WaitForSingleObject(StartEvent,INFINITE);
	ResetEvent(StartEvent);

	//启动成功
	m_bRun=true;
	return true;
}

//停止服务
bool CAFCDataBaseManage::Stop()
{
	//设置数据
	bool bFlush=m_bRun;
	m_bRun=false;
	m_DataLine.SetCompletionHandle(NULL);

	//关闭完成端口
	if (m_hCompletePort!=NULL)
	{
		//退出处理线程
		if (m_hThread!=NULL) 
		{
			PostQueuedCompletionStatus(m_hCompletePort,0,NULL,NULL);
			CloseHandle(m_hThread);
			m_hThread=NULL;
		}
		CloseHandle(m_hCompletePort);
		m_hCompletePort=NULL;
	}

	//关闭数据库连接
	if (m_pKernelInfo!=NULL)
	{
		sqlCloseConnection(hDBLogon);
		sqlCloseConnection(hDBNative);
	}

	return true;
}

//取消初始化
bool CAFCDataBaseManage::UnInit()
{
	//停止服务
	if (m_bRun) Stop();
	m_DataLine.CleanLineData();

	//恢复数据
	m_bInit=false;
	m_pInitInfo=NULL;
	m_hThread=NULL;
	m_hCompletePort=NULL;
	m_pKernelInfo=NULL;
	m_pHandleService=NULL;

	//if (m_pConnection->State)
	//	m_pConnection->Close();
	return true;
}

//加入处理队列
bool CAFCDataBaseManage::PushLine(DataBaseLineHead * pData, UINT uSize, UINT uHandleKind, UINT uIndex,DWORD dwHandleID)
{
	//处理数据
	pData->dwHandleID=dwHandleID;
	pData->uIndex=uIndex;
	pData->uHandleKind=uHandleKind;
	return (m_DataLine.AddData(&pData->DataLineHead,uSize,0)!=0);
}

//数据库处理线程
unsigned __stdcall CAFCDataBaseManage::DataServiceThread(LPVOID pThreadData)
{
	//数据定义
	ThreadStartStruct		* pData=(ThreadStartStruct *)pThreadData;		//线程启动数据指针
	CAFCDataBaseManage		* pDataManage=pData->pDataManage;				//数据库管理指针
	CDataLine				* pDataLine=&pDataManage->m_DataLine;			//数据队列指针
	IDataBaseHandleService	* pHandleService=pDataManage->m_pHandleService;	//数据处理接口
	HANDLE					hCompletionPort=pData->hCompletionPort;			//完成端口

	//线程数据读取完成
	::SetEvent(pData->hEvent);

	//重叠数据
	void					* pIOData=NULL;									//数据
	DWORD					dwThancferred=0;								//接收数量
	ULONG					dwCompleteKey=0L;								//重叠 IO 临时数据
	LPOVERLAPPED			OverData;										//重叠 IO 临时数据

	//数据缓存
	BOOL					bSuccess=FALSE;
	BYTE					szBuffer[LD_MAX_PART];

	while (TRUE)
	{
		//等待完成端口
		bSuccess=GetQueuedCompletionStatus(hCompletionPort,&dwThancferred,&dwCompleteKey,(LPOVERLAPPED *)&OverData,INFINITE);
		if ((bSuccess==FALSE)||dwThancferred==0)
		{
			_endthreadex(0);
			return 0;
		}
		while(pDataLine->GetDataCount())
		{
			DebugPrintf("pDataLine->GetDataCount()=%d",pDataLine->GetDataCount());
			try
			{
				//处理完成端口数据
				if (pDataLine->GetData((DataLineHead *)szBuffer,sizeof(szBuffer))<sizeof(DataBaseLineHead))
					continue;
				pHandleService->HandleDataBase((DataBaseLineHead *)szBuffer);
			}
			catch (...) 
			{ 
				TRACE("CATCH:%s with %s\n",__FILE__,__FUNCTION__);
				continue; 
			}
		}
	}

	_endthreadex(0);
	return 0;
}

///重联数据库
bool CAFCDataBaseManage::SQLConnectReset()
{
	if (m_bsqlInit == FALSE)
	{
		DWORD dw=cfgOpenFile("BZGameLocal.BCF");
		if(dw<0x10)
			return 0;
		m_sqlClass=cfgGetValue(dw,TEXT("DATABASECLASS"),TEXT("SQLCLASS"),0);
		CString sqlSection=TEXT("SQLSERVER");

		if(m_sqlClass==DB_TYPE_MYSQL)
			sqlSection=TEXT("MYSQL");

		m_szServer=cfgGetValue(dw,sqlSection,TEXT("DBSERVER"),TEXT(""));
		m_szAccount=cfgGetValue(dw,sqlSection,TEXT("DBACCOUNT"),TEXT(""));
		m_szPassword=cfgGetValue(dw,sqlSection,TEXT("DBPASSWORD"),TEXT(""));
		m_szDatabase=cfgGetValue(dw,sqlSection,TEXT("DATABASE"),TEXT(""));
		m_nPort=cfgGetValue(dw,sqlSection,TEXT("DBPORT"),0);
		m_szDetectTable=cfgGetValue(dw,sqlSection,TEXT("DETECTTABLE"),TEXT(""));
		cfgCloseAll();
		m_bsqlInit = TRUE;
	}
	if(m_pKernelInfo->bNativeDataBase)
	{
		sqlCloseConnection(hDBNative);
		hDBNative = sqlAddConnection( m_szServer, m_szAccount, m_szPassword, m_szDatabase,m_nPort, m_sqlClass, m_szDetectTable);
		if(hDBNative<(HANDLE)0x1000)
		{
			hDBNative=0;
			throw new CAFCException(TEXT("CAFCDataBaseManage::Start 连接本地数据库失败"),0x424);
		}
	}

	if(m_pKernelInfo->bLogonDataBase)
	{
		sqlCloseConnection(hDBLogon);
		hDBLogon = sqlAddConnection( m_szServer, m_szAccount, m_szPassword, m_szDatabase,m_nPort, m_sqlClass, m_szDetectTable);
		if(hDBLogon<(HANDLE)0x1000)
		{
			hDBLogon=0;
			throw new CAFCException(TEXT("CAFCDataBaseManage::Start 连接登陆数据库失败"),0x425);
		}
	}
}

//检测数据连接
bool CAFCDataBaseManage::CheckSQLConnect()
{

	return true;
	//if ((m_bRun==true)&&(m_pKernelInfo->bStartSQLDataBase==TRUE))
	//{
	//	//变量定义
	//	HRESULT hResult=S_OK;
	//	bool bReConnect=false;

	//	//检测登陆数据库
	//	if (m_pKernelInfo->bLogonDataBase==TRUE)
	//	{
	//		if ((m_DataBaseLogon.Execute(TEXT("select top 1 * from TGameServerInfo"),hResult)==false)&&(hResult!=DB_E_ERRORSINCOMMAND))
	//		{
	//			bReConnect=true;
	//			m_DataBaseLogon.Close();
	//			m_DataBaseLogon.Open(m_strLogonDataConnect);
	//		}
	//	}

	//	//检测本地数据库
	//	if (m_pKernelInfo->bNativeDataBase==TRUE)
	//	{
	//		if ((m_DataBaseNative.Execute(TEXT("select top 1 * from TGameServerInfo"),hResult)==false)&&(hResult!=DB_E_ERRORSINCOMMAND))
	//		{
	//			bReConnect=true;
	//			m_DataBaseNative.Close();
	//			m_DataBaseNative.Open(m_strNativeDataConnect);
	//		}
	//	}

	//	return bReConnect;
	//}

	return false;
}

void CAFCDataBaseManage::SQLConnect()
{
	if (m_bsqlInit == FALSE)
	{
		DWORD dw=cfgOpenFile("BZGameLocal.BCF");
		if(dw<0x10)
			return ;
		m_sqlClass=cfgGetValue(dw,TEXT("DATABASECLASS"),TEXT("SQLCLASS"),0);
		CString sqlSection=TEXT("SQLSERVER");

		if(m_sqlClass==DB_TYPE_MYSQL)
			sqlSection=TEXT("MYSQL");

		//if(sqlClass==DB_TYPE_ORACLE)
		//	sqlSection=TEXT("ORACLE");

		m_szServer=cfgGetValue(dw,sqlSection,TEXT("DBSERVER"),TEXT(""));
		m_szAccount=cfgGetValue(dw,sqlSection,TEXT("DBACCOUNT"),TEXT(""));
		m_szPassword=cfgGetValue(dw,sqlSection,TEXT("DBPASSWORD"),TEXT(""));
		m_szDatabase=cfgGetValue(dw,sqlSection,TEXT("DATABASE"),TEXT(""));
		m_nPort=cfgGetValue(dw,sqlSection,TEXT("DBPORT"),0);
		m_szDetectTable=cfgGetValue(dw,sqlSection,TEXT("DETECTTABLE"),TEXT(""));
		cfgCloseAll();
		m_bsqlInit = TRUE;
	}


	CString str;
	if (m_nPort==0)
		str.Format("Provider=sqloledb;Network Library=DBMSSOCN;Initial Catalog=%s;User ID=%s;Password=%s;Data Source=%s"
		,m_szDatabase.GetBuffer(),m_szAccount.GetBuffer(),m_szPassword.GetBuffer(),m_szServer.GetBuffer());
	else
		str.Format("Provider=sqloledb;Network Library=DBMSSOCN;Initial Catalog=%s;User ID=%s;Password=%s;Data Source=%s,%d"
		,m_szDatabase.GetBuffer(),m_szAccount.GetBuffer(),m_szPassword.GetBuffer(),m_szServer.GetBuffer(),m_nPort);


	if (m_pConnection->State != adStateClosed)
		m_pConnection->Close();

	try
	{
		m_pConnection->Open(_bstr_t(str),"","",adModeUnknown);
	}
	catch(_com_error e)
	{
		DebugPrintf("连接数据库失败！警告");
		if (m_pConnection->State != adStateClosed)
			m_pConnection->Close();

	}
	
}

bool CAFCDataBaseManage::SQLExec(const TCHAR * szSQL)
{
	CString strSql;
	bool bCon = true;
	try
	{
		strSql.Format("select top 1 * from TGameServerInfo");
		BSTR bstrSQL = strSql.AllocSysString(); 
		m_pRecordset->Open(bstrSQL,(IDispatch*)m_pConnection,adOpenDynamic,adLockOptimistic,adCmdText);
		if (m_pRecordset-> State == adStateOpen) 
			m_pRecordset-> Close(); 
	}
	catch (...)
	{
		bCon = false;
	}

	if(!bCon)
	{
		SQLConnect();
	}


	bool bRet =  SUCCEEDED(m_pRecordset->Open(szSQL,(IDispatch*)m_pConnection,adOpenDynamic,adLockOptimistic,adCmdText));

	if ((m_pRecordset->GetState()!=adStateClosed)) m_pRecordset->Close();

	return bRet;

}

//***********************************************************************************************//

//构造函数
CDataBaseHandle::CDataBaseHandle(void)
{
	m_pInitInfo=NULL;
	m_pKernelInfo=NULL;
	m_pRusultService=NULL;
	m_pDataBaseManage=NULL;
}

//析构函数 
CDataBaseHandle::~CDataBaseHandle(void)
{
}

//设置参数
bool CDataBaseHandle::SetParameter(IDataBaseResultService * pRusultService, CAFCDataBaseManage * pDataBaseManage, ManageInfoStruct * pInitData, KernelInfoStruct * pKernelData)
{
	m_pInitInfo=pInitData;
	m_pKernelInfo=pKernelData;
	m_pRusultService=pRusultService;
	m_pDataBaseManage=pDataBaseManage;
	return true;
}



//初始化函数
bool CAFCDataBaseManage::Init(ManageInfoStruct * pInitInfo, KernelInfoStruct * pKernelInfo, 
							  IDataBaseHandleService * pHandleService,IDataBaseResultService * pResultService)
{
	//效验参数
	if ((this==NULL)||(m_bInit==true)||(m_bRun==true))	throw new CAFCException(TEXT("CAFCDataBaseManage::Init 状态效验失败"),0x408);

	//设置数据
	m_pInitInfo=pInitInfo;
	m_pKernelInfo=pKernelInfo;
	m_pHandleService=pHandleService;
	m_DataLine.CleanLineData();

	//设置数据
	m_bInit=true;
	return true;
}





//***********************************************************************************************//
