#include "StdAfx.h"


#include ".\serversocketselect.h"
#include "IPCControls.h"


CServerSocketSelect::CServerSocketSelect(void)
{
}

CServerSocketSelect::~CServerSocketSelect(void)
{
}


bool CServerSocketSelect::OnSocketAccept(SOCKET iSocket)
{
	char _Num[64];
	ZeroMemory(_Num, sizeof(_Num));
	sprintf(_Num, "xxx OnSocketAccept:%d", iSocket);
	OutputDebugString(_Num);

	IPC_Head _p;
	_p.wDataSize = 0;
	_p.wMainCmdID = IPC_MAIN_IPC_KERNEL;
	_p.wSubCmdID = IPC_SUB_IPC_CLIENT_CONNECT;
	_p.wVersion = IPC_VER;
	m_pIChannelMessageSink->OnChannelMessage(&_p, NULL, 0, HWND(iSocket));
	//IChannelMessageSink->OnIPCConnect(iSocket);
	return true;
}
//网络读取消息
bool CServerSocketSelect::OnSocketReadEvent(SOCKET iSocket, void* pNetData, UINT uDataSize)
{
	Net_MessageHead *pNetHead = (Net_MessageHead*)pNetData;
	UINT8* pData = NULL;
	pData  = (UINT8*)pNetData + sizeof(Net_MessageHead);
	return OnSocketRead(iSocket, pNetHead, pData);
}

//网络关闭消息
bool CServerSocketSelect::OnSocketCloseEvent(SOCKET iSocket)
{
	char _Num[64];
	ZeroMemory(_Num, sizeof(_Num));
	sprintf(_Num, "xxx OnSocketCloseEvent:%d", iSocket);
	OutputDebugString(_Num);
	
	IPC_Head _p;
	_p.wDataSize = 0;
	_p.wMainCmdID = IPC_MAIN_IPC_KERNEL;
	_p.wSubCmdID = IPC_SUB_IPC_CLIENT_CLOSE;
	_p.wVersion = IPC_VER;
	m_pIChannelMessageSink->OnChannelMessage(&_p, NULL, 0, HWND(iSocket));

	return true;
}

bool CServerSocketSelect::OnSocketRead(SOCKET iSocket, Net_MessageHead* pNetHead, UINT8* pNetData)
{
	char _Num[64];
	ZeroMemory(_Num, sizeof(_Num));
	sprintf(_Num, "xxx OnSocketReadEvent:%d, %d, %d", iSocket, pNetHead->dMainID, pNetHead->dAssistantID);
	OutputDebugStringA(_Num);
	IPC_Head _p;
	_p.wMainCmdID = pNetHead->dMainID;
	_p.wSubCmdID = pNetHead->dAssistantID;
	_p.wDataSize = pNetHead->uMessageSize - sizeof(Net_MessageHead);
	_p.wVersion = IPC_VER;
	m_pIChannelMessageSink->OnChannelMessage(&_p, pNetData, _p.wDataSize, HWND(iSocket));
	return true;
}