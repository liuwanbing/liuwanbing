#pragma once

//////////////////////////////////////////////////////////////////////////
///B服务器客户端类，负责存储，接收，发送与B服务器相关的数据
///2010.8.12 rongqiufen
//////////////////////////////////////////////////////////////////////////

#include "../../../SDK/include/socket/TCPClientSocket.h"
#include "CenterServerhead.h"
#include "CommandMessage.h"

#include <fstream>
#include <string>
using namespace std;


class CBClientSocket :
	public IClientSocketService
{
public:
	CBClientSocket(void);
public:
	~CBClientSocket(void);


	///实现IClientSocketService的三个接口
public:
	///网络读取消息
	bool OnSocketReadEvent(NetMessageHead* pNetHead, void* pNetData, UINT uDataSize, CTCPClientSocket* pClientSocket);
	///网络连接消息
	bool OnSocketConnectEvent(UINT uErrorCode, CTCPClientSocket* pClientSocket);
	///网络关闭消息
	bool OnSocketCloseEvent();

public:
	///连接到服务器
	bool ConnectToServer();


	///服务器返回的消息响应
public:
	///socket连接回调函数
	bool OnConnect(NetMessageHead* pNetHead, void* pNetData, UINT uDataSize, CTCPClientSocket* pClientSocket);
	///请求Z服务器参数
	bool OnRequreGamePara(NetMessageHead* pNetHead, void* pNetData, UINT uDataSize, CTCPClientSocket* pClientSocket);

private:
	CTCPClientSocket	m_ClientSocket;
	CenterServerMsg		m_par;				///Z服务器参数

	string				m_IP;				///B服务器IP

};
