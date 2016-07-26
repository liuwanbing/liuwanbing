#include "stdafx.h"
#include "AFCSocket.h"
#include "DistriManager.h"
#include "GM_MessageDefine.h"


static char *G_GetAppPath()
{
	static char s_Path[MAX_PATH];
	bool s_bIsReady = false;
	if (!s_bIsReady)
	{
		ZeroMemory(s_Path,sizeof(s_Path));
		DWORD dwLength=GetModuleFileNameA(GetModuleHandle(NULL), s_Path, sizeof(s_Path));
		char *p = strrchr(s_Path, '\\');
		if(p != NULL)	*p = '\0';
		s_bIsReady = true;
	}
	return s_Path;
}

static void ProgramPrintf(int nLevel, char *pTag, const char *p, ...)
{
	/*nLevel = 
	0 == Print All
	1 == Debug
	2 == Warning
	3 == Info */
	//if(nLevel < PRN_LEVEL)
	//{
	//	return;
	//}
	char szTimeStr[32];
	char szDateStr[32];
	char szFilename[256];
	SYSTEMTIME sysTime;
	GetLocalTime(&sysTime);
	GetTimeFormatA( LOCALE_USER_DEFAULT, TIME_FORCE24HOURFORMAT, &sysTime, "HH:mm:ss", szTimeStr, 32);
	GetDateFormatA( LOCALE_USER_DEFAULT, LOCALE_USE_CP_ACP, &sysTime, "yyyy-MM-dd", szDateStr, 32);
	wsprintf( szFilename, TEXT("%s\\Log\\%s_BZ%s.txt"), G_GetAppPath(), szDateStr, "ZServerManager");
	va_list arg;
	va_start( arg, p );
	FILE *fp = NULL;
	fopen_s(&fp, szFilename, "a" );
	if (NULL == fp)
	{
		return;
	}
	fprintf(fp,"[%s %s]--", szDateStr, szTimeStr);
	vfprintf(fp, p, arg );
	fprintf(fp,"\n");
	fclose(fp);
}


IDistriManagerBase *CreateDistriManager(IDistriManagerNetBase *pNet)
{
	static IDistriManagerBase *p = NULL;
	if (!p)
		p = new CDistriManager(pNet);
	return p;
}

void DestoryDistriManager(IDistriManagerBase *p)
{
	SafeDelete(p);
}

CDistriManager::CDistriManager(IDistriManagerNetBase *pNet)
{
	m_pNet = pNet;
	m_pSocket = NULL;
	m_bConnect = 0;
	m_nKeepAliveCount = 0;
	Initial();
}

CDistriManager::~CDistriManager()
{
	if (m_pSocket != NULL) 
		delete m_pSocket;
	m_pSocket = NULL;
}

INT32 CDistriManager::Initial()
{
	if(m_pSocket == NULL)
		m_pSocket = new CTCPClientSocket(this);

	if(m_pSocket != NULL)
	{		
		m_strHostIP = "";
		PHOSTENT hostinfo;
		char hostname[MAX_PATH];
		if (gethostname(hostname, sizeof(hostname)) == 0)
		{
			if ((hostinfo = gethostbyname(hostname)) != NULL)
			{
				m_strHostIP = inet_ntoa(*(struct in_addr *)*hostinfo->h_addr_list);
			}
		}
		if (m_strHostIP.IsEmpty() || !m_pNet)
		{
			return -1;
		}
		UINT32 uPort = CENTER_DISTRI_PORT;

	/*	DWORD cfgHandle=cfgOpenFile("BZGameLogon.BCF");
		if(cfgHandle<0x10)
			return -1;

		m_strHostIP = cfgGetValue(cfgHandle,"CENTERSERVER","ServerIP","127.0.0.1");
		UINT32 uPort = cfgGetValue(cfgHandle,"CENTERSERVER","ServerPort",CENTER_DISTRI_PORT);
		*/		
		if(!Connect(m_strHostIP.GetBuffer(m_strHostIP.GetLength()), uPort))
		{
			ProgramPrintf(3, "Distri", "CDistriManager::Connect Error:%d", WSAGetLastError());
		}
	}
	return 0;
}

INT32 CDistriManager::Connect(const char * szServerIP, UINT32 uPort)
{
	if (m_pSocket != NULL)
	{
		m_bConnect = 1;
		return m_pSocket->Connect(szServerIP, uPort);
	}
	return -1;
}


INT32 CDistriManager::Connect(UINT32 dwServerIP, UINT32 uPort)
{
	if (m_pSocket != NULL)
	{
		m_bConnect = 1;
		return m_pSocket->Connect(dwServerIP, uPort);
	}
	return -1;
}

INT32 CDistriManager::SendNormalData(void * pData, UINT uSize, UINT bMainID, UINT bAssistantID, UINT bHandleCode)
{
	if (m_pSocket != NULL)
	{ 
		m_pSocket->SendData(pData, uSize, bMainID, bAssistantID, bHandleCode);
	}
	return 0;
}


INT32 CDistriManager::SendBatchData(void * pData, UINT uSize, UINT bMainID, UINT bAssistantID, UINT bHandleCode)
{
	if (m_pSocket != NULL)
	{ 
		NetMessageHead head;
		ZeroMemory(&head, sizeof(head));
		head.bMainID = bMainID;
		head.bAssistantID = bAssistantID;
		head.bHandleCode = bHandleCode;
		int nBufLen = sizeof(bMainID)+sizeof(bAssistantID)+sizeof(bHandleCode)+sizeof(uSize)+uSize;
		char *pMsg = new char[nBufLen+1];
		ZeroMemory(pMsg, nBufLen+1);
		int nIndex = 0;

		memcpy(&pMsg[nIndex], &bMainID, sizeof(bMainID));
		nIndex+=sizeof(bMainID);

		memcpy(&pMsg[nIndex], &bAssistantID, sizeof(bAssistantID));
		nIndex+=sizeof(bAssistantID);

		memcpy(&pMsg[nIndex], &bHandleCode, sizeof(bHandleCode));
		nIndex+=sizeof(bHandleCode);

		memcpy(&pMsg[nIndex], &uSize, sizeof(uSize));
		nIndex+=sizeof(uSize);

		memcpy(&pMsg[nIndex], (char*)pData, uSize);

		m_pSocket->SendData(pMsg, nBufLen, MDM_ZD_PACKAGE, ASS_ZD_BATCH, 0);
	}
	return 0;
}

INT32 CDistriManager::SendData(UINT uUserID, void * pData, UINT uSize, UINT bMainID, UINT bAssistantID, UINT bHandleCode, DWORD dwHandleID)
{
	if (m_pSocket != NULL)
	{ 
		NetMessageHead head;
		ZeroMemory(&head, sizeof(head));
		head.bMainID = bMainID;
		head.bAssistantID = bAssistantID;
		head.bHandleCode = bHandleCode;
		int nBufLen = sizeof(bMainID)+sizeof(bAssistantID)+sizeof(bHandleCode)+sizeof(dwHandleID)+sizeof(uSize)+sizeof(uUserID)+uSize;
		char *pMsg = new char[nBufLen+1];
		ZeroMemory(pMsg, nBufLen+1);
		int nIndex = 0;

		memcpy(&pMsg[nIndex], &uUserID, sizeof(uUserID));
		nIndex+=sizeof(uUserID);

		memcpy(&pMsg[nIndex], &bMainID, sizeof(bMainID));
		nIndex+=sizeof(bMainID);

		memcpy(&pMsg[nIndex], &bAssistantID, sizeof(bAssistantID));
		nIndex+=sizeof(bAssistantID);

		memcpy(&pMsg[nIndex], &bHandleCode, sizeof(bHandleCode));
		nIndex+=sizeof(bHandleCode);

		memcpy(&pMsg[nIndex], &bHandleCode, sizeof(dwHandleID));
		nIndex+=sizeof(dwHandleID);

		memcpy(&pMsg[nIndex], &uSize, sizeof(uSize));
		nIndex+=sizeof(uSize);

		memcpy(&pMsg[nIndex], (char*)pData, uSize);

		m_pSocket->SendData(pMsg, nBufLen, MDM_ZD_PACKAGE, ASS_ZD_SINGLE, 0);

		delete []pMsg;
	}
	return 0;
}

INT32 CDistriManager::SendData(UINT uUserID, UINT bMainID, UINT bAssistantID, UINT bHandleCode, DWORD dwHandleID)
{
	if (m_pSocket != NULL)
	{
		NetMessageHead head;
		ZeroMemory(&head, sizeof(head));
		head.bMainID = bMainID;
		head.bAssistantID = bAssistantID;
		head.bHandleCode = bHandleCode;
		int nBufLen = sizeof(bMainID)+sizeof(bAssistantID)+sizeof(bHandleCode)+sizeof(dwHandleID)+sizeof(uUserID);
		char *pMsg = new char[nBufLen+1];
		ZeroMemory(pMsg, nBufLen+1);
		int nIndex = 0;
		UINT uSize = 0;

		memcpy(&pMsg[nIndex], &uUserID, sizeof(uUserID));
		nIndex+=sizeof(uUserID);

		memcpy(&pMsg[nIndex], &bMainID, sizeof(bMainID));
		nIndex+=sizeof(bMainID);

		memcpy(&pMsg[nIndex], &bAssistantID, sizeof(bAssistantID));
		nIndex+=sizeof(bAssistantID);

		memcpy(&pMsg[nIndex], &bHandleCode, sizeof(bHandleCode));
		nIndex+=sizeof(bHandleCode);

		memcpy(&pMsg[nIndex], &bHandleCode, sizeof(dwHandleID));
		nIndex+=sizeof(dwHandleID);

		memcpy(&pMsg[nIndex], &uSize, sizeof(uSize));
		nIndex+=sizeof(uSize);

		m_pSocket->SendData(pMsg, nBufLen, MDM_ZD_PACKAGE, ASS_ZD_SINGLE, 0);

		delete []pMsg;
	}
	return 0;
}

void CDistriManager::CloseSocket(INT32 nSocketID, bool bNotify)
{
	if (m_pSocket != NULL)
	{
		m_pSocket->CloseSocket(bNotify);
	}
}


bool CDistriManager::CheckReConnect()
{
	if (m_bConnect == 0 && m_pSocket != NULL)
	{
	/*	DWORD cfgHandle=cfgOpenFile("BZGameLogon.BCF");
		if(cfgHandle<0x10) return true;

		m_strHostIP = cfgGetValue(cfgHandle,"CENTERSERVER","ServerIP","127.0.0.1");
		UINT32 uPort = cfgGetValue(cfgHandle,"CENTERSERVER","ServerPort",CENTER_DISTRI_PORT);
		*/
		if (m_strHostIP.IsEmpty() || !m_pNet)
		{
			return -1;
		}
		UINT32 uPort = CENTER_DISTRI_PORT;

		if(!Connect(m_strHostIP.GetBuffer(m_strHostIP.GetLength()), uPort))
		{
			ProgramPrintf(3, "Distri", "CDistriManager::Connect Error:%d", WSAGetLastError());
		}		
	}
	return true;
}

bool CDistriManager::CheckKeepLive()
{
	if (m_bConnect == 2 && m_pSocket != NULL)
	{
		m_nKeepAliveCount++;
		m_pSocket->SendData(MDM_CONNECT,ASS_NET_TEST,0);
		if (m_nKeepAliveCount > 5)
		{
			OnSocketCloseEvent();
		}
	}
	return true;
}

///网络读取消息
bool CDistriManager::OnSocketReadEvent(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket)
{
	m_nKeepAliveCount=0;
	if ((pNetHead->bMainID==MDM_CONNECT)) 
	{
		if(pNetHead->bAssistantID==ASS_NET_TEST)
		{
			m_pSocket->SendData(MDM_CONNECT,ASS_NET_TEST,0);
		}
		else if(pNetHead->bAssistantID==ASS_CONNECT_SUCCESS)
		{
			m_bConnect = 2;
			MSG_S_ConnectSuccess * _p = (MSG_S_ConnectSuccess *)pNetData;
			if (_p != NULL)
			{
				m_pSocket->SetCheckCode(_p->i64CheckCode, SECRET_KEY);
			}

			TZServerInfo _t;
			_t.iZid = m_pNet->GetZid();
			_t.iServerPort = m_pNet->GetServerPort();
			m_pSocket->SendData(&_t, sizeof(TZServerInfo), MDM_ZD_PACKAGE,ASS_ZD_CONNECT,0);
		}
	}
	else if(pNetHead->bMainID == MDM_ZD_PACKAGE)
	{
		int  nIndex=0;
		UINT uSize;
		UINT bMainID;
		UINT bAssistantID;
		UINT bHandleCode;
		DWORD dwHandleID;

		if(pNetHead->bAssistantID == ASS_ZD_BATCH)
		{
			memcpy(&bMainID, &((char*)pNetData)[nIndex], sizeof(bMainID));
			nIndex+=sizeof(bMainID);

			memcpy(&bAssistantID, &((char*)pNetData)[nIndex], sizeof(bAssistantID));
			nIndex+=sizeof(bAssistantID);

			memcpy(&bHandleCode, &((char*)pNetData)[nIndex], sizeof(bHandleCode));
			nIndex+=sizeof(bHandleCode);

			memcpy(&uSize, &((char*)pNetData)[nIndex], sizeof(uSize));
			nIndex+=sizeof(uSize);

			m_pNet->SendBatchDataSelf(&((char*)pNetData)[nIndex], uDataSize-nIndex, bMainID, bAssistantID, bHandleCode);
		}
		else if(pNetHead->bAssistantID == ASS_ZD_SINGLE)
		{
			UINT uUserID;

			memcpy(&uUserID, &((char*)pNetData)[nIndex], sizeof(uUserID));
			nIndex+=sizeof(uUserID);

			memcpy(&bMainID, &((char*)pNetData)[nIndex], sizeof(bMainID));
			nIndex+=sizeof(bMainID);

			memcpy(&bAssistantID, &((char*)pNetData)[nIndex], sizeof(bAssistantID));
			nIndex+=sizeof(bAssistantID);

			memcpy(&bHandleCode, &((char*)pNetData)[nIndex], sizeof(bHandleCode));
			nIndex+=sizeof(bHandleCode);

			memcpy(&dwHandleID, &((char*)pNetData)[nIndex], sizeof(dwHandleID));
			nIndex+=sizeof(dwHandleID);

			memcpy(&uSize, &((char*)pNetData)[nIndex], sizeof(uSize));
			nIndex+=sizeof(uSize);

			ProgramPrintf(1, "Distri", "Z(ID=%d)收到B服务器消息:用户=%d,%d,%d", m_pNet->GetZid(), uUserID, bMainID, bAssistantID);

			NetMessageHead _P;
			_P.uMessageSize = uSize;
			_P.bMainID = bMainID;
			_P.bAssistantID = bAssistantID;
			_P.bHandleCode = bHandleCode;
			_P.bReserve = pNetHead->bReserve;

			if(pNetHead->bHandleCode == HDC_ZD_FAIL)
			{
				m_pNet->SendDataFail(uUserID, (uDataSize-nIndex == 0)?NULL:&((char*)pNetData)[nIndex], uDataSize-nIndex, bMainID, bAssistantID, bHandleCode, dwHandleID);
			}
			else
			{
				int nSockIndex = m_pNet->GetIndexByID(uUserID);
				if(nSockIndex >=0 )
				{

					m_pNet->OnSocketRead(&_P, (char*)pNetData + nIndex, uDataSize - 24, 0, nSockIndex, 0);//
						//nSockIndex, (uDataSize-nIndex == 0)?NULL:&((char*)pNetData)[nIndex], uDataSize-nIndex, bMainID, bAssistantID, bHandleCode, dwHandleID);
				}
			}
		}
		else if(pNetHead->bAssistantID == ASS_ZD_LOGON)
		{
			if(pNetHead->bHandleCode == HDC_ZD_KICK)
			{
				DWORD dwUserID = *((DWORD*)pNetData);
				m_pNet->KickUserSocket(dwUserID);
			}
		}
	}
	return true;
}
///网络连接消息
bool CDistriManager::OnSocketConnectEvent(UINT uErrorCode, CTCPClientSocket * pClientSocket)
{
	if (uErrorCode)
	{
		m_bConnect = 0;
	}
	else
	{
		m_bConnect = 2;
	}
	return true;
}
///网络关闭消息
bool CDistriManager::OnSocketCloseEvent()
{
	m_bConnect = 0; 
	return true;
}