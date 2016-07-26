#pragma once
#ifndef _USER_CUSTOM_LOGO_CLASS_HEADER_
#define _USER_CUSTOM_LOGO_CLASS_HEADER_

#include "UserlogoMessage.h"
#include "UserlogoDatabaseMessage.h"

class CUserCustomLogo
{
	CWnd    *pParentWnd;
public:
	explicit CUserCustomLogo(CWnd *pWnd){pParentWnd=pWnd;};
	void OnNetMessage(NetMessageHead * pNetHead, void * pData, UINT uSize, ULONG uAccessIP, UINT uIndex, DWORD dwHandleID);
	void OnUploadFace(void *pData,UINT uSize,UINT uIndex,DWORD dwHandleID);
public:
	~CUserCustomLogo(void);
public:
	void OnRequestLogoInformation(void * pData, UINT uSize, UINT uIndex, DWORD dwHandleID);
public:
	void OnDownloadRequest(void* pData, UINT uSize, UINT uIndex, DWORD dwHandleID);
public:
	void downloadCustomFace(long dwUserID, long nBlockIndex, UINT uIndex, DWORD dwHandleID);
public:
	void OnDownloadResult(void * pData, UINT uSize, UINT uIndex, DWORD dwHandleID);
};

#endif