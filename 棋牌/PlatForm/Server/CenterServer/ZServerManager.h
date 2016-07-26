#pragma once
//#include "e:\work\d_platform(2.2)分离\sdk\include\servercommon\mainmanage.h"

#include <vector>
#include <set>
using namespace std;

#include "basemessage.h"
#include "MainManage.h"



struct ZSvrNode_st
{
	CString szIPAddr;				//服务器Ip地址
	long	lPort;					//服务器端口
	int		nZid;					//服务器ID
	UINT	uSocket;				//Z服务器与B服务器的Socket号

	set<UINT>	onlineUserSocket;	//服务器在线用户


	ZSvrNode_st()
		:szIPAddr(_T(""))
		,lPort(0)
		,nZid(0)
		,uSocket(0)
	{
	}
};

class CMainserverList
{
	CString m_strHostIP;
	void clear();
	CRITICAL_SECTION cs;
public:
	vector<ZSvrNode_st> m_List;
	void Lock(){}
	void UnLock(){}

	//根据SocketID找相应服务器
	ZSvrNode_st* GetServerBySocket(UINT uSocket);
	//根据UserId找相应服务器
	ZSvrNode_st* find(DWORD dUserId);
	//添加一个Z服务器
	bool InsertServer(CString szIP, long lPort, int nZid, UINT uSocket);
	//删除一个Z服务器
	bool RemoveServer(UINT uSocket);
	//调试函数
	void PrintDebug();

	CMainserverList();
	~CMainserverList();
	bool RandAServer(char* ipaddr,long& port);
};


class CZServerManager :
	public CBaseMainManage
{
public:
	CZServerManager(void);
public:
	~CZServerManager(void);

private:
	//获取信息函数
	virtual bool PreInitParameter(ManageInfoStruct * pInitData, KernelInfoStruct * pKernelData);
	//SOCKET 数据读取
	virtual bool OnSocketRead(NetMessageHead * pNetHead, void * pData, UINT uSize, ULONG uAccessIP, UINT uIndex, DWORD dwHandleID);
	//SOCKET 关闭
	virtual bool OnSocketClose(ULONG uAccessIP, UINT uSocketIndex, long int lConnectTime);
	//数据库处理结果
	virtual bool OnDataBaseResult(DataBaseResultLine * pResultData){return true;}
	//定时器消息
	virtual bool OnTimerMessage(UINT uTimerID){return true;}

public:
	CMainserverList							m_MainserverList;

};