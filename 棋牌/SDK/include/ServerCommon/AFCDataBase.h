#ifndef AFCDATABASE_H_FILE_2003_07
#define AFCDATABASE_H_FILE_2003_07

#include <afx.h>
#include <afxdisp.h>
#include "../common/ComStruct.h"
#include "AFCInterFace.h"
#include "AFCLock.h"
#include "DataLine.h"
#include "AFCException.h"
#include "icrsint.h"
#include "adoid.h"
//#include <Icrsint.h>
//
//  //#import "C:\Program Files\common files\system\ado\msado15.dll" no_namespace rename("EOF","adoEOF")
//
//#pragma warning (disable: 4146)
//#import "MsAdo15.dll" rename_namespace("ADOCG") rename("EOF","EndOfFile")
//using namespace ADOCG;

#pragma warning(disable:4146)
#import "C:\Program Files\Common Files\System\ADO\msado15.dll" named_guids rename("EOF","adoEOF"), rename("BOF","adoBOF")
#pragma warning(default:4146)
using namespace ADODB;



///类说明
class CDataLine;
class CAFCDataBaseManage;

///***********************************************************************************************///


///***********************************************************************************************///

#ifndef STRUCT_DATABASELINEHEAD
#define STRUCT_DATABASELINEHEAD
///数据库数据包头结构
struct DataBaseLineHead
{
	DataLineHead					DataLineHead;							///队列头
	UINT							uHandleKind;							///处理类型
	UINT							uIndex;									///对象索引
	DWORD							dwHandleID;								///对象标识
};
#endif//STRUCT_DATABASELINEHEAD
///数据库处理类
class AFX_EXT_CLASS CAFCDataBaseManage
{
	///变量定义
protected:
	bool							m_bInit;								///初始化标志
	bool							m_bRun;									///运行标志
	IDataBaseHandleService			* m_pHandleService;						///数据处理接口

	///变量定义
public:

	HANDLE							hDBLogon;								///登录数据库
	HANDLE							hDBNative;								///本地数据库

	///信息变量
public:
	KernelInfoStruct				* m_pKernelInfo;						///内核数据
	ManageInfoStruct				* m_pInitInfo;							///初始化数据指针

	///信息变量
protected:
	///CString							m_strLogonDataConnect;					///登陆字符串
	///CString							m_strNativeDataConnect;					///本地字符串

	///内核变量
protected:
	HANDLE							m_hThread;								///线程句柄
	HANDLE							m_hCompletePort;						///完成端口
public:
	CDataLine						m_DataLine;								///数据队列

	///函数定义
public:
	///构造函数
	CAFCDataBaseManage();
	///析构函数
	virtual ~CAFCDataBaseManage(void);

	///服务函数
public:
	///初始化函数
	bool Init(ManageInfoStruct * pInitInfo, KernelInfoStruct * pKernelInfo, IDataBaseHandleService * pHandleService,IDataBaseResultService * pResultService);
	///取消初始化
	bool UnInit();
	///开始服务
	bool Start();
	///停止服务
	bool Stop();
	///加入处理队列
	bool PushLine(DataBaseLineHead * pData, UINT uSize, UINT uHandleKind, UINT uIndex,DWORD dwHandleID);

	///功能函数
public:
	///检测数据连接
	bool CheckSQLConnect();
	
	///重联数据库 2012-02-22 zht
	bool SQLConnectReset();

	int		m_sqlClass;
	CString	m_szServer;
	CString m_szAccount;
	CString m_szPassword;
	CString m_szDatabase;
	SHORT	m_nPort;
	CString m_szDetectTable;
	BOOL    m_bsqlInit;


	_ConnectionPtr m_pConnection;
	_RecordsetPtr m_pRecordset;
	void SQLConnect();
	bool SQLExec(const TCHAR * szSQL);

	///内部函数
private:
	///数据库处理线程
	static unsigned __stdcall DataServiceThread(LPVOID pThreadData);
	///日志数据库处理线程
	static unsigned __stdcall DataLogThread(LPVOID pThreadData);
};

///***********************************************************************************************///

///数据库处理接口类
class AFX_EXT_CLASS CDataBaseHandle : public IDataBaseHandleService
{
	///变量定义
protected:
	KernelInfoStruct						* m_pKernelInfo;				///内核数据
	ManageInfoStruct						* m_pInitInfo;					///初始化数据指针
	IDataBaseResultService					* m_pRusultService;				///结果处理接口
	CAFCDataBaseManage						* m_pDataBaseManage;			///数据库对象

	///函数定义
public:
	///构造函数
	CDataBaseHandle(void);
	///析构函数
	virtual ~CDataBaseHandle(void);

	///接口函数
public:
	///设置参数
	virtual bool SetParameter(IDataBaseResultService * pRusultService, CAFCDataBaseManage * pDataBaseManage, ManageInfoStruct * pInitData, KernelInfoStruct * pKernelData);

	///功能函数
};

///***********************************************************************************************///

#endif