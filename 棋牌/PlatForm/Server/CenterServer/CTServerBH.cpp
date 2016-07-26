#include "StdAfx.h"
#include "ctserverbh.h"
#include "../common/commonuse.h"
#include "server0.h"

void CCTServer::OnAccept(ISSocket* pclient)
{
	///pclient->Send(&m_msgSendToClient,sizeof(CenterServerMsg));
}
void CCTServer::OnReceive(ISSocket* pclient,const void* recvbuf,int size)
{
	if(size == 1)
	{
		char c=*(char *)recvbuf;
		pclient->Send(&m_msgSendToClient,sizeof(CenterServerMsg));
		((IClient*)pclient)->CloseMe(IClient::ONTIMEOUT);
	}
}

void CCTServer::OnInitServer ()
{
	nPort=CCenterHead::CenterServerPort;
	m_SendBufCount=10;//500;
	m_PlayerBufSize=10;//500;
	m_MaxPlayerCount=10;//500;
	m_QueueCount=10;//500;
	
	SendBuffer.SetBufCount (m_SendBufCount);

	char s[400];
	m_PlayerBufSize = m_MaxPlayerCount+100;
	sprintf(s,"最大选手数: %d,最大选手缓冲区数: %d\n",m_MaxPlayerCount, m_PlayerBufSize);
	OutPut(s);

	
	sprintf(s,"最大客户端连接数: %d\n", GetMaxClientCount());
	OutPut(s);

	sprintf(s,"最大发送缓冲区数量=%d,每个发送缓冲区字节数=%d",m_SendBufCount,BUFFER_SIZE);
	OutPut(s);

	 m_Queue.InitSize(m_QueueCount);
	 sprintf(s,"接收缓冲区消息队列数: %d\n", m_QueueCount);
	OutPut(s);
}
ISSocket* CCTServer::NewAClient(int index)
{
	CClientSocket *p=new CClientSocket;
	p->m_pServer=this;
	p->Init(m_listener);
	p->m_PlayerIndex=index;
	return p;
}

void CCTServer::RandAServer()
{
	m_MainserverList.RandAServer (m_msgSendToClient.m_strMainserverIPAddr,
		m_msgSendToClient.m_iMainserverPort);
}

bool CCTServer::CMainserverList::RandAServer(char* ipaddr,long& port)
{
	bool bb=false;
	Lock();
	if(m_List.IsEmpty ())goto ret;
	{
	 int count=m_List.GetCount ();
	 long t=GetCurrentTime() + rand();
	 int curi=(t) % count ;int i=0;
	 for(POSITION pos=m_List.GetHeadPosition ();pos !=NULL;)
	 {
		 Node* nd=(Node*)m_List.GetNext (pos);
		 if(nd)
		 {
			 CString s=nd->IPAddr ;
		   if(i == curi)
		   {
			   port=nd->Port ;///20);
			   memcpy(ipaddr,s.GetBuffer (s.GetLength ()),20);
			   bb=true;
			   break;
		   }
		 }
		 i++;
	 }
	}
ret:
	UnLock();
	return bb;
}


void CCTServer::CMainserverList::clear()
{
	Lock();
	while(!m_List.IsEmpty())
	{
		Node* pdesk = (Node*)m_List.RemoveHead();
		delete pdesk;
	}
	m_List.RemoveAll();
	UnLock();
}

CCTServer::CMainserverList::~CMainserverList()
{
	clear();
}

void CCTServer::CMainserverList::ReadINIFile()
{
	clear();
	CString s=CINIFile::GetAppPath ();
	CINIFile f( s + "TMLCenterServer.ini");
	int count=f.GetKeyVal ("System","m_MainserverListCount",0);
	for(int i=0;i<count;i++)
	{
		CString s;s.Format ("Mainserver%d",i+1);
		CString ip=f.GetKeyVal (s,"IPAddr","127.0.0.34");
		Node* nd=new Node();
		nd->IPAddr =ip;
		long port=f.GetKeyVal (s,"Port",0);
		nd->Port =port;
		port=f.GetKeyVal (s,"id",0);
		nd->id  =port;
		Lock();
		  m_List.AddTail (nd);
		UnLock();

	}
}

CCTServer::CMainserverList::CMainserverList()
{
	///srand( (unsigned)time( NULL ) );
	InitializeCriticalSection(&cs);
    ReadINIFile();
}

void	CCTServer::OnClose(ISSocket* client)
{
	OutPut( " a socket CClient disconnected.\n");
}


void CCTServer::WriteINIFile()
{
	char buf[30];
	_ltoa(m_msgSendToClient.m_iMainserverPort,buf,10);

	CString s=CINIFile::GetAppPath (),ss;
	CINIFile f( s + "TMLCenterServer.ini");
	f.SetKeyValString ("System","m_iMainserverPort",buf);
	f.SetKeyValString ("System","TML_SerialNO",m_msgSendToClient.m_strGameSerialNO);
	f.SetKeyValString ("System","m_strMainserverIPAddr",m_msgSendToClient.m_strMainserverIPAddr);
	f.SetKeyValString ("System","m_strHomeADDR",m_msgSendToClient.m_strHomeADDR);
	f.SetKeyValString ("System","m_strLoginNewUserADDR",m_msgSendToClient.m_strLoginNewUserADDR);
	f.SetKeyValString ("System","m_strHelpADDR",m_msgSendToClient.m_strHelpADDR);
	f.SetKeyValString ("System","m_strBBSADDR",m_msgSendToClient.m_strBBSADDR);
	f.SetKeyValString ("System","m_strHuaZhangADDR",m_msgSendToClient.m_strHuaZhangADDR);
	f.SetKeyValString ("System","m_strDownLoadSetupADDR",m_msgSendToClient.m_strDownLoadSetupADDR);
	f.SetKeyValString ("System","m_strDownLoadUpdatepADDR",m_msgSendToClient.m_strDownLoadUpdatepADDR);
}
CCTServer::CCTServer(void):CServer0()
{///m_msgSendToClient
//	g_pCCTServer=this;

#define len(l1,l2)  ( (l1).GetLength () > (l2 ) ? (l2) : ((l1).GetLength ()))
	
	CString s=CINIFile::GetAppPath (),ss;
	CINIFile f( s + "TMLCenterServer.ini");
	////主服务器端口号
	m_msgSendToClient.m_iMainserverPort=f.GetKeyVal ("System","m_iMainserverPort",6800);
	ss=f.GetKeyVal ("System","TML_SerialNO","13012608286");////客户端当前版本系列号
	memcpy(m_msgSendToClient.m_strGameSerialNO,ss,len(ss,20)  );//ss.GetLength ());
	ss=f.GetKeyVal ("System","m_strMainserverIPAddr","123");////主服务器IP地址

	memcpy(m_msgSendToClient.m_strMainserverIPAddr,ss,len(ss,20)  );
	ss=f.GetKeyVal ("System","m_strHomeADDR","123");////主页WEB地址
	memcpy(m_msgSendToClient.m_strHomeADDR,ss,len(ss,50)  );
	ss=f.GetKeyVal ("System","m_strLoginNewUserADDR","123");////注册页WEB地址
	memcpy(m_msgSendToClient.m_strLoginNewUserADDR,ss,len(ss,50)  );
	ss=f.GetKeyVal ("System","m_strHelpADDR","123");////帮助页WEB地址
	memcpy(m_msgSendToClient.m_strHelpADDR,ss,len(ss,50)  );
	ss=f.GetKeyVal ("System","m_strBBSADDR","123");////论坛页WEB地址
	memcpy(m_msgSendToClient.m_strBBSADDR,ss,len(ss,50)  );
	ss=f.GetKeyVal ("System","m_strHuaZhangADDR","123");////游戏卡划帐页WEB地址
	memcpy(m_msgSendToClient.m_strHuaZhangADDR,ss,len(ss,50)  );
	ss=f.GetKeyVal ("System","m_strDownLoadSetupADDR","123");////客户端安装程序下载页WEB地址
	memcpy(m_msgSendToClient.m_strDownLoadSetupADDR,ss,len(ss,50)  );
	ss=f.GetKeyVal ("System","m_strDownLoadUpdatepADDR","123");////客户端更新程序下载页WEB地址
	memcpy(m_msgSendToClient.m_strDownLoadUpdatepADDR,ss,len(ss,50)  );
}

CCTServer::~CCTServer(void)
{
	
}











/*void CCTServer::CClient::RegMsgFun()
{
	////set_msgfunc(CCenterHead::CenterType_ClientSend, (msg_func_t)update_chatstring);
///	set_msgfunc(CenterType_ClientEnter, (msg_func_t)set_clientname);
}
*/

/*
CCTServer* g_pCCTServer=NULL;

CBuffer* GetSendBuf()
{
	ASSERT(g_pCCTServer);
	return &g_pCCTServer->SendBuffer;
}
*/
/*

void	CCTServer::CClient::SendHello()
{
	CCTServer *sv=(CCTServer*)m_pserver ;
	prepare_send(sizeof(CenterServerMsg));
	sv->RandAServer();
	memcpy((char*)i_sendbuf, (const char*)&sv->m_msgSendToClient ,sizeof(CenterServerMsg));
	prepare_handle_list(enumSendToWho_Me);
	confirm_msg();
	///set_curop(op_sendallclient);
	send_allclient();
	//OnClose();
	//delete this;
}


void CCTServer::NewAAcceptSocket ()
{
///////	new CAcptSkt(port, g_recvbuf_size,this);
}

void CCTServer::CClient::on_complete(BOOL pm_ok, uint32_t pm_size)
{
	if ( get_curop() == op_sendallclient )
	{
		//send_allclient();
		//set_curop(op_saybyebye);
		//write(i_handle, NULL, 0);
		//OnClose();
		//delete this;
		return;
	}
	else if (  !pm_ok || pm_size <= 0)
	{
		//this->m_pserver->CloseASocket(this);
		//if ( !( send_allclient()) )
		{
		OnClose();
		///remove(this);//delete this;
		delete this;
		}
		return;
	}

}
*/