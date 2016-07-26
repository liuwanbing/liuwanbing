#pragma once

////#include "server0.h"
//#include "IClient.h"
///#include "acceptsocket.h"

///#include "../common/head.h"
class CCTServer:public CServer0
{
protected:
	virtual void OnInitServer ();
	virtual ISSocket* NewAClient(int index=0);
public:
	void RandAServer();
	void WriteINIFile();
	CenterServerMsg m_msgSendToClient;
	CCTServer(void);
	~CCTServer(void);
public:
	virtual void OnReceive(ISSocket* pclient,const void* recvbuf,int size);
	virtual void OnAccept(ISSocket* pclient);
	virtual void	OnClose(ISSocket* client);
//protected:
	//virtual void NewAAcceptSocket ();



public:

/*	class CAcptSkt:public CAcceptSocket
	{
	public:
		virtual void NewAClient(HANDLE skt)
		{
			new CClient( skt, i_size ,m_pserver);
		}

		CAcptSkt(uint16_t pm_port,
			      uint32_t pm_size,
				  CServer0 *pserver
				  )
				  :CAcceptSocket(pm_port, pm_size,pserver)
		{}
		~CAcptSkt(){}
	};


	class CClient:public IClient
	{
	protected:
		virtual void on_complete(BOOL pm_ok, uint32_t pm_size);
		void	SendHello();
		virtual void RegMsgFun();
	public:
		CClient(
			HANDLE pm_handle, 
			uint32_t pm_size,
			CServer0 *pserver
				  )
				  :IClient(pm_handle, pm_size,pserver)
		{
			SendHello();
		}
		~CClient(){}

	};

*/

	class CMainserverList
	{
		void clear();
		CRITICAL_SECTION cs;
		void ReadINIFile();
	public:
		struct Node
		{
			CString IPAddr;
			long Port;
			int id;
		};
		CPtrList m_List;
		void Lock(){EnterCriticalSection(&cs);}
		void UnLock(){LeaveCriticalSection(&cs);}

		CMainserverList();
		~CMainserverList();
		bool RandAServer(char* ipaddr,long& port);
	};
	CMainserverList m_MainserverList;
};


//extern CCTServer* g_pCCTServer;

//extern CBuffer* GetSendBuf();