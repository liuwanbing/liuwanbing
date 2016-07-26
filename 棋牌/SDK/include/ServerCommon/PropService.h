#pragma once

#include "PropDatabaseMessage.h"
class CPropService
{
	CWnd *pParentWnd;
public:
	 explicit CPropService(CWnd *pWnd);
public:
	~CPropService(void);
public:
	bool OnNetMessage(NetMessageHead *pNetHead, void* pData, UINT uSize, ULONG uAccessIP, UINT uIndex, DWORD dwHandleID);
	bool OnUseProp(NetMessageHead *pNetHead, void* pData, UINT uSize, ULONG uAccessIP, UINT uIndex, DWORD dwHandleID);
	bool OnUserSmallBoardcase(NetMessageHead *pNetHead, void* pData, UINT uSize, ULONG uAccessIP, UINT uIndex, DWORD dwHandleID);

	bool OnDataBaseResult(DataBaseResultLine * pResultData);
	bool OnUsePropResult(DataBaseResultLine *pResultData);
	bool OnUserSmallBoardcaseResult(DataBaseResultLine *pResultData);
};
