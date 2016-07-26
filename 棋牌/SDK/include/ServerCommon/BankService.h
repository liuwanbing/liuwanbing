#pragma once

#include "AFCDataBase.h"

#include "BankMessage.h"



class CBankService
{
public:
	CBankService(CWnd *pWnd);
public:
	~CBankService(void);
public:
	CWnd			*m_pWnd;
	//CGameMainManage *m_pGameMainManage;
public:
	bool OnNetMessage(NetMessageHead *pNetHead, void* pData, UINT uSize, ULONG uAccessIP, UINT uIndex, DWORD dwHandleID);
	bool OnDataBaseResult(DataBaseResultLine * pResultData); 
};

