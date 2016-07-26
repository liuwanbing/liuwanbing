//////////////////////////////////////////////////////////////////////////
// 用于GM功能的接口定义头文件
// GM功能的具体实现由具体实现类完成
// 创建：曾宪冬
// 日期：20100722
//////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef __INTERFACE_FOR_GM_H
#define __INTERFACE_FOR_GM_H

#include "GM_MessageDefine.h"

///
/// GM消息处理接口，GM消息处理都由此接口完成，
/// 为便于释放，声明为纯虚类而不是interface
///
class CGMMessageBase
{
public:
	/// 构造函数
	CGMMessageBase(){};
	/// 析构函数
	virtual ~CGMMessageBase(){};

public:
	/// 接口定义
	/// 消息处理函数
	virtual bool OnSocketMessage(NetMessageHead * pNetHead, void * pData, UINT uSize, ULONG uAccessIP, UINT uIndex, DWORD dwHandleID) = 0;
	///数据库结果处理
	virtual bool OnDataBaseResultEvent(DataBaseResultLine * pResultData)=0;
private:
	CGMMessageBase(const CGMMessageBase&);
	CGMMessageBase &operator=(const CGMMessageBase&);
};

///
/// GM功能调用接口，CGameLogonManage和CGameMainManage类须实现本接口中的函数
/// GM消息处理类处理完消息后，需要完成功能的具体实现，
/// 具体实现由原有CGameLogonManage和CGameMainManage完成，
/// 这两个类需要从本接口派生，本接口指针须提供给GM消息处理类
///
interface IGMFunction
{
public:
	/// 向客户端发送数据
	virtual int SendData(UINT uIndex, BYTE bMainID, BYTE bAssistantID, BYTE bHandleCode, DWORD dwHandleID) = 0;
	/// 向客户端发送数据
	virtual int SendData(UINT uIndex, void * pData, UINT uBufLen, BYTE bMainID, BYTE bAssistantID, BYTE bHandleCode, DWORD dwHandleID) = 0;
	/// 像其他玩家发送数据
	virtual int SendDataByUser(UINT uUserID, BYTE bMainID, BYTE bAssistantID, UINT bHandleCode, DWORD dwHandleID) = 0;
	/// 像其他玩家发送数据
	virtual int SendDataByUser(UINT uUserID, void * pData, UINT uBufLen, BYTE bMainID, BYTE bAssistantID, UINT bHandleCode, DWORD dwHandleID) = 0;
	/// 向客户端发送广播数据
	virtual int SendBatchData(void * pData, UINT uBufLen, BYTE bMainID, BYTE bAssistantID, BYTE bHandleCode) = 0;
	/// 根据用户ID获取Socket序号
	virtual int GetIndexByID(int iUserID)=0;
	/// 向数据库模块发送访问请求
	virtual bool PushDataLine(DataBaseLineHead * pData, UINT uSize, UINT uHandleKind, UINT uIndex,DWORD dwHandleID) = 0;
	/// 判断某连接是否还存在
	virtual bool IsConnectID(UINT uIndex, DWORD dwHandleID) = 0;
	/// 判断某玩家是否已经登录
	virtual bool IsUserAlreadyLogon(DWORD dwUserID) = 0;
	/// 添加玩家登录记录
	virtual bool AddLogonUser(DWORD dwUserID, UINT uIndex, bool bIsGM = false) = 0;
};


/// 创建GM消息处理对象
extern CGMMessageBase *CreateGMModule(IGMFunction *);
/// 销毁GM消息处理对象
extern void DestroyGMModule(CGMMessageBase *);

#endif//__INTERFACE_FOR_GM_H