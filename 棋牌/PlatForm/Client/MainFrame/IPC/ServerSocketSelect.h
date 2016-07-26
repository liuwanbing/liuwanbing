#pragma once

#include "xsocketbase.h"
#include <Windows.h>
#include <stdio.h>

#include "sink.h"


////客户端网络服务接口
////interface IServerSocketService
////{
////	接口函数 
////public:
////	////////////////////////////////////////////////////////////////////////
////	virtual bool OnSocketAccept(SOCKET iSocket) = 0;
////	网络读取消息
////	virtual bool OnSocketReadEvent(SOCKET iSocket, void * pNetData, UINT uDataSize)=0;
////	网络关闭消息
////	virtual bool OnSocketCloseEvent(SOCKET iSocket)=0;
////};

struct Net_MessageHead
{
	DWORD						uMessageSize;						///数据包大小
	DWORD						dMainID;							///处理主类型
	DWORD						dAssistantID;						///辅助处理类型 ID
	DWORD						dHandleCode;						///数据包处理代码
	DWORD						dReserve;							///保留字段
};


class CServerSocketSelect: public IServerSocketService
{
public:
	CServerSocketSelect(void);
	~CServerSocketSelect(void);
public:
	bool OnSocketAccept(SOCKET iSocket);
	//网络读取消息
	bool OnSocketReadEvent(SOCKET iSocket, void * pNetData, UINT uDataSize);
	//网络关闭消息
	bool OnSocketCloseEvent(SOCKET iSocket);

public:
	bool OnSocketRead(SOCKET iSocket, Net_MessageHead* pNetHead, UINT8* pNetData);

	void SetChannel(IChannelMessageSink *pIChannel) {m_pIChannelMessageSink = pIChannel;}
public:
	IChannelMessageSink				* m_pIChannelMessageSink;				//回调接口
};
