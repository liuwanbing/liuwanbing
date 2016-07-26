#pragma once

#ifndef AFCDATABASE_H_FILE_2003_07
#define AFCDATABASE_H_FILE_2003_07

#include "StdAfx.h"
#include <afx.h>
#include <afxdisp.h>
#include <Icrsint.h>
#include "ComStruct.h"
#include "AFCInterFace.h"
#include "AFCLock.h"
#include "DataLine.h"
#include "AFCException.h"

#pragma warning (disable: 4146)
#import "MsAdo15.dll" rename_namespace("ADOCG") rename("EOF","EndOfFile")
using namespace ADOCG;

//类型定义
typedef	_com_error CComError;

//类说明
class CDataLine;
class CAFCRecord;
class CAFCDataBase;
class CAFCDataBaseManage;

//数据库名字
const TCHAR		szCenterDataBaseName[]=TEXT("AFCDataCenter");	//中心数据数据库名字
const TCHAR		szLogonDataBaseName[]=TEXT("AFCUsers");			//登陆数据数据库名字
const TCHAR		szNativeDataBaseName[]=TEXT("AFCLocal");		//本地缓存数据库名字

//缓冲数据表
//const TCHAR		szBufferPoint[]=TEXT("BufferPoint");				//分数缓冲表
//const TCHAR		szBufferExperience[]=TEXT("BufferExperience");		//经验缓冲表

//公共使用表
const TCHAR		szLogonTable[]=TEXT("Users");					//登陆用户表
const TCHAR		szIPRuleTable[]=TEXT("GameIPRule");					//IP 规则限制表
const TCHAR		szLogonMoney[]=TEXT("ComLogonMoney");				//金币类游戏状态信息表
const TCHAR		szMatchUserInfo[]=TEXT("MatchUserInfo");			//比赛信息表


//static CString stc_sCenterDataConnect="Data Source=TML";
//static CString stc_sLogonDataConnect="Data Source=TML";
//static CString stc_sNativeConnect="Data Source=TML";////"Data Source=AFCLocal";
static CString stc_sCenterDataConnect="FILE NAME=bzgame.udl";
static CString stc_sLogonDataConnect="FILE NAME=bzgame.udl";
static CString stc_sNativeConnect="FILE NAME=bzgame.udl";////"Data Source=AFCLocal";

const bool stc_bUseACCESSDb=true;
const bool stc_bUseODBCConnect=true;

//***********************************************************************************************//


//数据库类
class EXT_CLASS CAFCDataBase
{
	friend class CAFCRecord;

	//变量定义
private:
	//bool								m_bShowError;				//是否显示错误
	///_ConnectionPtr						m_pConnection;				//数据库实例
	
	//函数定义
public:
	//构造函数
	CAFCDataBase(void);
	//析构函数
	virtual ~CAFCDataBase(void);

	//功能函数
public:
	//错误模式
	bool ShowError(bool bShowError);
	//是否打开
	bool IsOpen();
	//打开连接
	bool Open(const TCHAR * pszConnection);
	//关闭连接
	bool Close();
	//执行语句
	bool Execute(const TCHAR * szSQL);
	//执行语句
	bool Execute(const TCHAR * szSQL, HRESULT & hResult);

	//内部函数
private:
	//报告错误
	void ShowErrorMessage(CComError & ADOError);
};

//***********************************************************************************************//

//数据集类
class EXT_CLASS CAFCRecord
{
	//变量定义
private:
	bool						m_bShowError;				//是否显示错误
	_RecordsetPtr				m_pRecordset;				//记录实例
	CAFCDataBase				* m_pDataConnect;			//数据连接

	//函数定义
public:
	//构造函数
	CAFCRecord(CAFCDataBase * ADODataBase);
	//析构函数
	virtual ~CAFCRecord(void);

	//辅助函数
protected:
	//报告错误
	void ShowErrorMessage(CComError &ADOError);

	//功能函数
public:
	//错误模式
	bool ShowError(bool bShowError);
	//获取记录集数目
	DWORD GetRecordCount();
	//执行查询
	bool Open(const TCHAR * szSQL);
	//执行查询
	bool Open(const TCHAR * szSQL, HRESULT & hResult);
	//关闭数据集
	bool Close();
	//是否记录集未
	bool IsEnd() { return (m_pRecordset->EndOfFile==VARIANT_TRUE); };
	//移动到下一列
	void MoveNext() { m_pRecordset->MoveNext(); }
	//获取参数
	bool GetFieldValue(LPCTSTR lpFieldName, BYTE & bValue);
	//获取参数
	bool GetFieldValue(LPCTSTR lpFieldName, int & nValue);
	//获取参数
	bool GetFieldValue(LPCTSTR lpFieldName, long & lValue);
	//获取参数
	bool GetFieldValue(LPCTSTR lpFieldName, unsigned long & ulValue);
	//获取参数
	bool GetFieldValue(LPCTSTR lpFieldName, UINT & ulValue);
	//获取参数
	bool GetFieldValue(LPCTSTR lpFieldName, __int64 & llValue);
	//获取参数
	bool GetFieldValue(LPCTSTR lpFieldName, double & dbValue);
	//获取参数
	bool GetFieldValue(LPCTSTR lpFieldName, TCHAR szBuffer[], UINT uSize);
	//获取参数
	bool GetFieldValue(LPCTSTR lpFieldName, COleDateTime & Time);
	//获取参数
	bool GetFieldValue(LPCTSTR lpFieldName, bool & bValue);
};

//***********************************************************************************************//

//数据库数据包头结构
struct DataBaseLineHead
{
	DataLineHead					DataLineHead;							//队列头
	UINT							uHandleKind;							//处理类型
	UINT							uIndex;									//对象索引
	DWORD							dwHandleID;								//对象标识
};

//数据库处理类
class EXT_CLASS CAFCDataBaseManage
{
	//变量定义
protected:
	bool							m_bInit;								//初始化标志
	bool							m_bRun;									//运行标志
	IDataBaseHandleService			* m_pHandleService;						//数据处理接口

	//变量定义
public:
	CAFCDataBase					m_DataBaseCenter;						//中心数据库
	CAFCDataBase					m_DataBaseLogon;						//登陆数据库
	CAFCDataBase					m_DataBaseNative;						//本地数据库

	//信息变量
public:
	KernelInfoStruct				* m_pKernelInfo;						//内核数据
	ManageInfoStruct				* m_pInitInfo;							//初始化数据指针

	//信息变量
protected:
	CString							m_strLogonDataConnect;					//登陆字符串
	CString							m_strNativeDataConnect;					//本地字符串
	CString							m_strCenterDataConnect;					//中心字符串

	//内核变量
protected:
	HANDLE							m_hThread;								//线程句柄
	HANDLE							m_hCompletePort;						//完成端口
	CDataLine						m_DataLine;								//数据队列

	//函数定义
public:
	//构造函数
	CAFCDataBaseManage();
	//析构函数
	virtual ~CAFCDataBaseManage(void);

	//服务函数
public:
	//初始化函数
	bool Init(ManageInfoStruct * pInitInfo, KernelInfoStruct * pKernelInfo, IDataBaseHandleService * pHandleService,IDataBaseResultService * pResultService);
	//取消初始化
	bool UnInit();
	//开始服务
	bool Start();
	//停止服务
	bool Stop();
	//加入处理队列
	bool PushLine(DataBaseLineHead * pData, UINT uSize, UINT uHandleKind, UINT uIndex,DWORD dwHandleID);

	//功能函数
public:
	//检测数据连接
	bool CheckSQLConnect();

	//内部函数
private:
	//数据库处理线程
	static unsigned __stdcall DataServiceThread(LPVOID pThreadData);
	//日志数据库处理线程
	static unsigned __stdcall DataLogThread(LPVOID pThreadData);
};

//***********************************************************************************************//

//数据库处理接口类
class EXT_CLASS CDataBaseHandle : public IDataBaseHandleService
{
	//变量定义
protected:
	KernelInfoStruct						* m_pKernelInfo;				//内核数据
	ManageInfoStruct						* m_pInitInfo;					//初始化数据指针
	IDataBaseResultService					* m_pRusultService;				//结果处理接口
	CAFCDataBaseManage						* m_pDataBaseManage;			//数据库对象

	//函数定义
public:
	//构造函数
	CDataBaseHandle(void);
	//析构函数
	virtual ~CDataBaseHandle(void);

	//接口函数
public:
	//设置参数
	virtual bool SetParameter(IDataBaseResultService * pRusultService, CAFCDataBaseManage * pDataBaseManage, ManageInfoStruct * pInitData, KernelInfoStruct * pKernelData);

	//功能函数
public:
	//是否手机号码
	bool IsMobileNum(TCHAR * szChar);
};

//***********************************************************************************************//

#endif