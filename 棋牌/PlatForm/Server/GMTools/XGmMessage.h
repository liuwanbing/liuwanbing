#pragma once
#include "InterfaceForGM.h"


// Get individual number
#define FIRST_IPADDRESS(x)  ((x>>24) & 0xff)
#define SECOND_IPADDRESS(x) ((x>>16) & 0xff)
#define THIRD_IPADDRESS(x)  ((x>>8) & 0xff)
#define FOURTH_IPADDRESS(x) (x & 0xff)


/// 
/// GM消息处理模块类定义
class XGmMessageModule :
	public CGMMessageBase
{
	/// 功能实现接口，用于访问数据库，及下发数据
	IGMFunction *m_pFunction;

private:
	/// 无参构造函数、拷贝构造函数，赋值操作符都不可用
	XGmMessageModule();
	XGmMessageModule(const XGmMessageModule &);
	XGmMessageModule & operator=(const XGmMessageModule &);

public:
	XGmMessageModule(IGMFunction *pFunction);
	virtual ~XGmMessageModule(void);

public:
	/// 接口定义
	/// 消息处理函数
	virtual bool OnSocketMessage(NetMessageHead * pNetHead, void * pData, UINT uSize, ULONG uAccessIP, UINT uIndex, DWORD dwHandleID);
	///数据库结果处理
	virtual bool OnDataBaseResultEvent(DataBaseResultLine * pResultData);
private:

	//////////////////////////////////////////////////////////////////////////
	/// Socket处理过程

	/// 奖励玩家方案
	bool OnGetAwardType(NetMessageHead * pNetHead, void * pData, UINT uSize, ULONG uAccessIP, UINT uIndex, DWORD dwHandleID);
	/// GM取得指定玩家的被处罚记录
	bool OnGetPunishedUser(NetMessageHead * pNetHead, void * pData, UINT uSize, ULONG uAccessIP, UINT uIndex, DWORD dwHandleID);
	/// 对玩家进行操作
	bool OnOperateUserByID(NetMessageHead * pNetHead, void * pData, UINT uSize, ULONG uAccessIP, UINT uIndex, DWORD dwHandleID);
	/// 发布消息或向某特定玩家发消息
	bool OnGMSendMessage(NetMessageHead * pNetHead, void * pData, UINT uSize, ULONG uAccessIP, UINT uIndex, DWORD dwHandleID);
	///// 取在线玩家
	//bool OnGetLogonUsers(NetMessageHead * pNetHead, void * pData, UINT uSize, ULONG uAccessIP, UINT uIndex, DWORD dwHandleID);
	/// 取得道具ID与名称对应关系
	bool OnGetPropInfo(NetMessageHead * pNetHead, void * pData, UINT uSize, ULONG uAccessIP, UINT uIndex, DWORD dwHandleID);
	/// gm登录
	bool OnGMLogonOn(NetMessageHead * pNetHead, void * pData, UINT uSize, ULONG uAccessIP, UINT uIndex, DWORD dwHandleID);


	//////////////////////////////////////////////////////////////////////////
	/// 数据库操作返回后处理

	/// GM获取指定玩家的历史被操作记录
	bool OnGMGetPunishedUserResult(DataBaseResultLine *pResultData);
	/// GM登录结果
	bool OnGMLogonOnResult(DataBaseResultLine *pResultData);
	/// GM获取奖励方案结果
	bool OnGMGetAwardTypeListResult(DataBaseResultLine *pResultData);
	/// GM奖惩玩家结果
	bool OnGMPutOperationUserResult(DataBaseResultLine *pResultData);
	/// GM获取道具ID与名称对应关系结果
	bool OnGMGetPropInfoResult(DataBaseResultLine *pResultData);

	/// IP地址转换为字符串
	TCHAR * ULongToIP(ULONG dwIP, TCHAR * szIP);
	TCHAR * GetTimeString(TCHAR *szTime, int iMinutes);
};

