#pragma once
#ifndef _USER_IM_CLASS_HEADER_
#define _USER_IM_CLASS_HEADER_

#include "IMMessage.h"
#include "IMDatabaseMessage.h"

#include <bitset>
using namespace std;

class CIMService
{
	CWnd	*pParentWnd;

	//添加屏蔽消息的内容插件 yjj 090317
	CTalkMessageFilter  m_TalkFilter;

public:
	explicit CIMService(CWnd *pWnd);//{pParentWnd=pWnd;};
public:
	~CIMService(void);
public:
	void OnNetMessage(NetMessageHead * pNetHead, void * pData, UINT uSize, ULONG uAccessIP, UINT uIndex, DWORD dwHandleID);
public:
	void OnNetGetUserCount(void * pData, UINT uSize, UINT uIndex, DWORD dwHandleID);
	void OnNetUserMessage(void * pData, UINT uSize, UINT uIndex, DWORD dwHandleID);
	void OnNetAddRequest(NetMessageHead * pNetHead, void * pData, UINT uSize, UINT uIndex, DWORD dwHandleID);
	void OnNetAddResponse(void * pData, UINT uSize, UINT uIndex, DWORD dwHandleID);
	void OnNetUserSetGroup(void * pData, UINT uSize, UINT uIndex, DWORD dwHandleID);

	void OnNetAddRequest_Z(NetMessageHead * pNetHead, void * pData, UINT uSize, UINT uIndex, DWORD dwHandleID);
};

#endif