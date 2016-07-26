#pragma once

#include "AfxTempl.h"
#include "MainManage.h"
#include "GamePlaceMessage.h"

#include "ZServerManager.h"

//宏定义 
#define TID_UPDATE_LIST							50					//更新列表 ID
#define LS_TIME_OUT								60000L				//列表超时数

/******************************************************************************************************/

//游戏登陆管理类
class CCenterServerManage : public CBaseMainManage
{
	//变量定义
public:
///	CServerGameListManage						m_GameList;						//游戏列表

	//函数定义
public:
	//构造函数
	CCenterServerManage(void);
	//析构函数
	virtual ~CCenterServerManage(void);
	//获取游戏列表
///	CServerGameListManage * GetGameList() { return &m_GameList; };

	//功能函数
public:
	//数据管理模块启动
	virtual bool OnStart();
	//数据管理模块关闭
	virtual bool OnStop();

	//服务扩展接口函数
private:
	///数据管理模块初始化
	virtual bool OnInit(ManageInfoStruct * pInitData, KernelInfoStruct * pKernelData);
	///数据管理模块卸载
	virtual bool OnUnInit();
	//获取信息函数
	virtual bool PreInitParameter(ManageInfoStruct * pInitData, KernelInfoStruct * pKernelData);
	//SOCKET 数据读取
	virtual bool OnSocketRead(NetMessageHead * pNetHead, void * pData, UINT uSize, ULONG uAccessIP, UINT uIndex, DWORD dwHandleID);
	//SOCKET 关闭
	virtual bool OnSocketClose(ULONG uAccessIP, UINT uSocketIndex, long int lConnectTime);
	//数据库处理结果
	virtual bool OnDataBaseResult(DataBaseResultLine * pResultData){return true;}
	//定时器消息
	virtual bool OnTimerMessage(UINT uTimerID);

	void RandAServer();
	void GetINIFile();

	///< 功能 从配置文件中读取URL
	///< @param strKey
	///< @Return void
	void GetURL(char* strKey);

	/// 从Function.bcf中读取功能配置
	/// @param void
	/// @return void
	void GetFunction();
	

	class CMainserverList
	{
		void clear();
		CRITICAL_SECTION cs;
	//public:	void ReadINIFile(CString TMLkey);
	public:
		void LoadServerList(DWORD cfgHandle);
		struct Node
		{
			CString IPAddr;
			long Port;
			int id;
		};
		CPtrList m_List;
		void Lock(){}//EnterCriticalSection(&cs);}
		void UnLock(){}//LeaveCriticalSection(&cs);}

		CMainserverList();
		~CMainserverList();
		bool RandAServer(char* ipaddr,long& port);
	};
	//CMainserverList m_MainserverList;
public:
	CenterServerMsg m_msgSendToClient;////游戏全局参数
	CZServerManager m_ZServerManager;

};

/******************************************************************************************************/

//游戏登陆模块
class CCenterServerModule : public IModuleManageService
{
	//变量定义 
public:
	CCenterServerManage						m_LogonManage;					//登陆数据管理

	//函数定义
public:
	//构造函数
	CCenterServerModule(void);
	//析构函数
	virtual ~CCenterServerModule(void);

	//接口函数
public:
	//初始化函数
	virtual bool InitService(ManageInfoStruct * pInitData);
	//卸载函数
	virtual bool UnInitService();
	//开始函数
	virtual bool StartService(UINT &errCode);
	//停止函数
	virtual bool StoptService();
	//删除函数
	virtual bool DeleteService();
};

/******************************************************************************************************/
