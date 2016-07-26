#pragma once

#ifndef AFC_INTERFACE_FILE
#define AFC_INTERFACE_FILE

/********************************************************************************************/

///服务接口定义

/********************************************************************************************/

class CTCPSocket;					///<SOCKET 类
class CTCPSocketForZ;				//SOCKET 类为Z服务器特制 Add by ZXD 20090804
class CTCPClientSocket;				///<SOCKET 类
class CAFCDataBaseManage;			///<数据库模块类
struct NetMessageHead;				///<网络数据包头
struct ManageInfoStruct;			///<初始化信息结构
struct KernelInfoStruct;			///<内核信息结构
struct DataLineHead;				///<队列头结构
struct DataBaseLineHead;			///<数据库队列头结构
struct DataBaseResultLine;			///<数据库结果头结构

///服务器网络服务接口 
interface IServerSocketService
{
	///接口函数
public:
	///网络关闭处理
	virtual bool OnSocketCloseEvent(ULONG uAccessIP, UINT uIndex, long int lConnectTime)=0;
	///网络消息处理
	virtual bool OnSocketReadEvent(CTCPSocket * pSocket, NetMessageHead * pNetHead, void * pData, UINT uSize, UINT uIndex, DWORD dwHandleID)=0;
};
//服务器网络服务接口 
interface IServerSocketServiceForZ
{
	//接口函数
public:
	//网络关闭处理
	virtual bool OnSocketCloseEvent(ULONG uAccessIP, UINT uIndex, long int lConnectTime)=0;
	//网络消息处理
	virtual bool OnSocketReadEvent(CTCPSocketForZ * pSocket, NetMessageHead * pNetHead, void * pData, UINT uSize, UINT uIndex, DWORD dwHandleID)=0;
};

///客户端网络服务接口
interface IClientSocketService
{
	///接口函数 
public:
	///网络读取消息
	virtual bool OnSocketReadEvent(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket)=0;
	///网络连接消息
	virtual bool OnSocketConnectEvent(UINT uErrorCode, CTCPClientSocket * pClientSocket)=0;
	///网络关闭消息
	virtual bool OnSocketCloseEvent()=0;
};

///数据库结果服务接口
interface IDataBaseResultService
{
	///接口函数
public:
	///数据库结果处理
	virtual bool OnDataBaseResultEvent(DataBaseResultLine * pResultData, UINT uHandleKind, UINT uHandleResult, UINT uResultSize, UINT uIndex, DWORD dwHandleID)=0;
};

///数据库处理服务接口
interface IDataBaseHandleService
{
	///接口函数
public:
	///设置参数
	virtual bool SetParameter(IDataBaseResultService * pRusultService, CAFCDataBaseManage * pDataBaseManage, ManageInfoStruct * pInitData, KernelInfoStruct * pKernelData)=0;
	///数据库处理接口
	virtual UINT HandleDataBase(DataBaseLineHead * pSourceData)=0;
};

///组件管理接口
interface IModuleManageService
{
	///接口函数
public:
	///初始化函数 
	virtual bool InitService(ManageInfoStruct * pInitData)=0;
	///取消初始化函数 
	virtual bool UnInitService()=0;
	///初始化函数 
	virtual bool StartService(UINT &errCode)=0;
	///初始化函数 
	virtual bool StoptService()=0;
	///删除函数
	virtual bool DeleteService()=0;
};

/********************************************************************************************/

#endif