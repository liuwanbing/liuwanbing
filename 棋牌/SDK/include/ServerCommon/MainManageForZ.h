#pragma once

#ifndef MAINMANAGEFORZ_H_FILE
#define MAINMANAGEFORZ_H_FILE

#include "AFCInterface.h"
#include "../Common/ComStruct.h"
#include "DataLine.h"
#include "AFCSocket.h"
#include "AFCDataBase.h"
#include "TalkMessageFilter.h"

//基础数据管理类
class AFX_EXT_CLASS CBaseMainManageForZ : public IServerSocketServiceForZ , public IDataBaseResultService
{
	//控件变量
protected:
	bool									m_bRun;						//运行标志
	bool									m_bInit;					//初始化标志
	HWND									m_hWindow;					//窗口句柄
	HANDLE									m_hWindowThread;			//窗口线程
	HANDLE									m_hHandleThread;			//处理线程
	HANDLE									m_hCompletePort;			//处理完成端口
	CDataLine								m_DataLine;					//数据队列

public:
	ManageInfoStruct						m_InitData;					//初始化数据
	KernelInfoStruct						m_KernelData;				//内核数据
	ServerDllInfoStruct						m_DllInfo;					//DLL 信息
	CTCPSocketManageForZ					m_TCPSocket;				//网络模块
	CAFCDataBaseManage						m_SQLDataManage;			//数据库模块


	//添加过滤组件
	CTalkMessageFilter                     m_TalkFilter;     //语言过滤系统   yjj 090317


	//函数定义
public:
	//构造函数
	CBaseMainManageForZ(void);
	//析构函数
	virtual ~CBaseMainManageForZ(void);

	//公共使用功能函数 （本处理线程调用）
public:	
	//设定定时器
	bool SetTimer(UINT uTimerID, UINT uElapse);
	//清除定时器
	bool KillTimer(UINT uTimerID);

	//服务接口函数 （主线程调用）
public:
	//初始化函数
	virtual bool Init(ManageInfoStruct * pInitData, IDataBaseHandleService * pDataHandleService);
	//取消初始化函数
	virtual bool UnInit();
	//启动函数
	virtual bool Start();
	//停止服务
	virtual bool Stop();

	//服务接口函数 （主线程调用）
private:
	//获取信息函数 （必须重载）
	virtual bool PreInitParameter(ManageInfoStruct * pInitData, KernelInfoStruct * pKernelData)=0;
	//数据管理模块初始化
	virtual bool OnInit(ManageInfoStruct * pInitData, KernelInfoStruct * pKernelData) { return true; };
	//数据管理模块卸载
	virtual bool OnUnInit() { return true; };
	//数据管理模块启动
	virtual bool OnStart() { return true; };
	//数据管理模块关闭
	virtual bool OnStop() { return true; };

	//服务扩展接口函数 （本处理线程调用）
private:
	//SOCKET 数据读取 （必须重载）
	virtual bool OnSocketRead(NetMessageHead * pNetHead, void * pData, UINT uSize, ULONG uAccessIP, UINT uIndex, DWORD dwHandleID)=0;
	//SOCKET 关闭 （必须重载）
	virtual bool OnSocketClose(ULONG uAccessIP, UINT uSocketIndex, long int lConnectTime)=0;
	//数据库处理结果 （必须重载）
	virtual bool OnDataBaseResult(DataBaseResultLine * pResultData)=0;
	//定时器消息 （必须重载）
	virtual bool OnTimerMessage(UINT uTimerID)=0;

	//服务接口函数 （其他服务线程调用）
public:
	//网络关闭处理 （不需要重载）
	virtual bool OnSocketCloseEvent(ULONG uAccessIP, UINT uIndex, long int lConnectTime);
	//网络消息处理 （不需要重载）为Z服务器准备
	virtual bool OnSocketReadEvent(CTCPSocketForZ * pSocket, NetMessageHead * pNetHead, void * pData, UINT uSize, UINT uIndex, DWORD dwHandleID);
	//数据库结果处理 （不需要重载）
	virtual bool OnDataBaseResultEvent(DataBaseResultLine * pResultData, UINT uHandleKind, UINT uHandleResult, UINT uResultSize, UINT uIndex, DWORD dwHandleID);

	//内部函数（主线程调用）
private:
	//内核启动函数
	bool AFCKernelStart();
	//定时器通知消息
	bool WindowTimerMessage(UINT uTimerID);
	//队列数据处理线程
	static unsigned __stdcall LineDataHandleThread(LPVOID pThreadData);
	//WINDOW 消息循环线程
	static unsigned __stdcall WindowMsgThread(LPVOID pThreadData);
	//窗口回调函数
	static LRESULT CALLBACK WindowProcFunc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};

#endif