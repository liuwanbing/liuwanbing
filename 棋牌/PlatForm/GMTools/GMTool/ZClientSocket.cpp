#include "StdAfx.h"
#include "ZClientSocket.h"



#pragma comment(lib,"Iphlpapi.lib")	//锁定机器 zxj 2009-11-12

CZClientSocket::CZClientSocket(void): m_ClientSocket(this)
{
}

CZClientSocket::~CZClientSocket(void)
{
	for (int i = 0; i < m_KindList.size(); i++) delete m_KindList[i];
	for (int i = 0; i < m_NameList.size(); i++) delete m_NameList[i];
	for (int i = 0; i < m_RoomList.size(); i++) delete m_RoomList[i];
	for (map<int, GM_AwardType_t*>::iterator i = m_AwardType.begin(); i != m_AwardType.end(); i++) 
		delete i->second;
	for (map<int, GM_GetPropInfo_t*>::iterator i = m_PropList.begin(); i != m_PropList.end(); i++) 
		delete i->second;
	m_ClientSocket.CloseSocket(false);
}


//网络读取消息
bool CZClientSocket::OnSocketReadEvent(NetMessageHead* pNetHead, void* pNetData, UINT uDataSize, CTCPClientSocket* pClientSocket)
{
	switch (pNetHead->bMainID)
	{
	case MDM_CONNECT:	///连接消息 
		{
			return OnConnect(pNetHead, pNetData, uDataSize, pClientSocket);
		}
	case MDM_GP_LOGON:	///登录
		{
			return OnLongo(pNetHead, pNetData, uDataSize, pClientSocket);
		}
	case MDM_GP_LIST:	///获取游戏列表
		{
			return OnGetGameList(pNetHead, pNetData, uDataSize, pClientSocket);
		}
	case MDM_GAMEMASTER_MESSAGE:	///GM消息
		{
			return OnGMMessage(pNetHead, pNetData, uDataSize, pClientSocket);
		}
	case MDM_GP_PLAYER_2_GM:	///GM私聊公告消息
		{
			return OnTalkMsg(pNetHead, pNetData, uDataSize, pClientSocket);
		}
	}
	return true;
}

//网络连接消息
bool CZClientSocket::OnSocketConnectEvent(UINT uErrorCode, CTCPClientSocket* pClientSocket)
{
	if (uErrorCode!=0)	///连接出现错误
	{
		AfxMessageBox("连接Z服务器失败!");
		PostQuitMessage(0);
		return true;
	}
	return true;///连接成功
}
//网络关闭消息
bool CZClientSocket::OnSocketCloseEvent()
{
	AfxMessageBox("与服务器断开连接请重新登陆!");
	PostQuitMessage(0);
	return true;
}

bool CZClientSocket::ConnectToServer()
{
	return m_ClientSocket.Connect(m_ServerInfo.m_strMainserverIPAddr, m_ServerInfo.m_iMainserverPort);
}


bool CZClientSocket::OnConnect(NetMessageHead* pNetHead, void* pNetData, UINT uDataSize, CTCPClientSocket* pClientSocket)
{
	if (pNetHead->bAssistantID == ASS_NET_TEST)	///网络测试信息
	{
		pClientSocket->SendData(MDM_CONNECT,ASS_NET_TEST,0);///连接消息类型
		return true;
	}
	else if ((pNetHead->bAssistantID == 2) || (pNetHead->bAssistantID == 3))	///连接成功
	{
		MSG_S_ConnectSuccess * _p = (MSG_S_ConnectSuccess *)pNetData;
		if (_p != NULL)
		{
			pClientSocket->SetCheckCode(_p->i64CheckCode, SECRET_KEY);
		}

		return AfxGetMainWnd()->PostMessage(MSG_CMD_CONNET_Z_SUCCEED, 0, 0);
	}
}

bool CZClientSocket::OnLongo(NetMessageHead* pNetHead, void* pNetData, UINT uDataSize, CTCPClientSocket* pClientSocket)
{
	if (pNetHead->bHandleCode == ERR_GP_LOGON_SUCCESS)
	{
		memset(&m_LogonInfo, 0, sizeof(m_LogonInfo));
		m_LogonInfo = *((GM_MSG_O_LogonResult_t *)pNetData);
		CopyMemory(m_LogonInfo.szMD5Pass, m_LogonByName.szMD5Pass, sizeof(m_LogonByName.szMD5Pass));

		m_ClientSocket.SendData(MDM_GAMEMASTER_MESSAGE, ASS_GM_MESSAGE_AWARD_TYPE, 0);
		m_ClientSocket.SendData(MDM_GAMEMASTER_MESSAGE, ASS_GM_MESSAGE_PROP_INFO, 0);
		m_GetAwardTypeing = true;
		m_GetAwardListing = true;
	} 
	else //if (pNetHead->bAssistantID == ASS_GP_LOGON_ERROR)
	{
		//pClientSocket->CloseSocket(false);
		AfxMessageBox(ERR_RP_STRING[pNetHead->bHandleCode]);
		AfxGetMainWnd()->PostMessage(MSG_CMD_CONNET_Z_FAIL, 0, 0);
		return true;
	}
	return true;
}


bool CZClientSocket::OnGetGameList(NetMessageHead* pNetHead, void* pNetData, UINT uDataSize, CTCPClientSocket* pClientSocket)
{
	switch (pNetHead->bAssistantID)
	{
	case ASS_GP_LIST_KIND:
		{
			int n = uDataSize / sizeof(ComKindInfo);
			ComKindInfo *data = (ComKindInfo*)pNetData;
			for (int i = 0; i < n; i++)
			{
				for (int j = 0; j < m_KindList.size(); j++) 
					if (data->uKindID == m_KindList[j]->uKindID) break;

				ComKindInfo *p = new ComKindInfo;
				*p= *data;
				m_KindList.push_back(p);
				data++;
			}
			if (pNetHead->bHandleCode == ERR_GP_LIST_FINISH) AfxGetMainWnd()->PostMessage(MSG_CMD_GET_GAMEKIND, 0, 0);
			break;
		}
	case ASS_GP_LIST_NAME:
		{
			int n = uDataSize / sizeof(ComNameInfo);
			ComNameInfo *data = (ComNameInfo*)pNetData;
			for (int i = 0; i < n; i++)
			{
				for (int j = 0; j < m_NameList.size(); j++) 
					if (data->uNameID == m_NameList[j]->uNameID) break;

				ComNameInfo *p = new ComNameInfo;
				*p= *data;
				m_NameList.push_back(p);
				data++;
			}
			if (pNetHead->bHandleCode == ERR_GP_LIST_FINISH) AfxGetMainWnd()->PostMessage(MSG_CMD_GET_GAMENAME, 0, 0);
			break;
		}
	case ASS_GP_LIST_ROOM:
		{
			if (uDataSize<sizeof(MSG_GP_SR_GetRoomStruct)) return false;
			MSG_GP_SR_GetRoomStruct * pNetRoomBuf=(MSG_GP_SR_GetRoomStruct *)pNetData;


			int n = (uDataSize - sizeof(MSG_GP_SR_GetRoomStruct)) / sizeof(ComRoomInfo);
			ComRoomInfo *data = (ComRoomInfo*)((char*)pNetData+sizeof(MSG_GP_SR_GetRoomStruct));
			for (int i = 0; i < n; i++)
			{
				bool have = false;
				for (int j = 0; j < m_RoomList.size(); j++) 
				{
					if (data->uRoomID == m_RoomList[j]->uRoomID) 
					{
						have = true;
						break;
					}
				}
				if (!have) 
				{
					ComRoomInfo *p = new ComRoomInfo;
					*p= *data;
					m_RoomList.push_back(p);
				}
				data++;
			}
			if (pNetHead->bHandleCode == ERR_GP_LIST_FINISH) 
				AfxGetMainWnd()->PostMessage(MSG_CMD_GET_GAMEROOM, 0, 0);
			break;
		}
	case ASS_GP_LIST_COUNT:		//在线人数
		{
			if (uDataSize<sizeof(DL_GP_RoomListPeoCountStruct)) return false;
			UINT uCount = 0;
			uCount = uDataSize/sizeof(DL_GP_RoomListPeoCountStruct);
			uCount = uCount<0?0:uCount;
			DL_GP_RoomListPeoCountStruct* pPeoCount = (DL_GP_RoomListPeoCountStruct*)pNetData;
			
			if (uCount > 0)
			{

			
				if(pNetHead->bHandleCode == 0)
				{////大厅房间人数
					for(int i = 0; i < uCount; i++)
					{
						for (int j = 0; j < m_RoomList.size(); j++)
						{
							if (pPeoCount->uID == m_RoomList[j]->uRoomID)
								m_RoomList[j]->uPeopleCount = pPeoCount->uOnLineCount; 
						}
					}
				}
				else
				{////游戏名称人数
					for(int i = 0; i < uCount; i++)
					{
						for (int j = 0; j < m_NameList.size(); j++)
						{
							if (pPeoCount->uID == m_NameList[j]->uNameID)
								m_NameList[j]->m_uOnLineCount = pPeoCount->uOnLineCount; 
						}
					}
				}
				AfxGetMainWnd()->PostMessage(MSG_CMD_UPDATE_GAMEPEO, 0, 0);
			}
		}
	}
	return true;
}


bool CZClientSocket::OnGMMessage(NetMessageHead* pNetHead, void* pNetData, UINT uDataSize, CTCPClientSocket* pClientSocket)
{
	switch (pNetHead->bAssistantID)
	{
	case ASS_GM_MESSAGE_GM_LOGON:
		{
			//AfxMessageBox("成功");
			return OnLongo(pNetHead, pNetData, uDataSize, pClientSocket);
			break;
		}
	case ASS_GM_MESSAGE_AWARD_TYPE:					///< 获取奖励方案
		{
			if (!m_GetAwardTypeing) break;
			int nCount = uDataSize / sizeof(GM_AwardType_t);
			if (nCount > 0)
			{
				GM_AwardType_t * pAward = (GM_AwardType_t *)pNetData;
				for (int i = 0; i < nCount; i++)
				{
					if (m_AwardType.count(pAward->iSeriNo) == 0)
					{
						GM_AwardType_t * p = new GM_AwardType_t;
						*p = *pAward;
						m_AwardType[p->iSeriNo] = p;
					}
					pAward++;
				}
			}
			if (pNetHead->bHandleCode == HC_GM_GET_LIST_END) m_GetAwardTypeing = false;
			if (!m_GetAwardTypeing && !m_GetAwardListing) GetGameList();
			break;
		}

	case ASS_GM_MESSAGE_OPERATE_HISTORY:
		{
			GM_OperatedPlayerInfo_t* finduser = (GM_OperatedPlayerInfo_t*)pNetData;
			if (pNetHead->bHandleCode == 1)
			{
				m_GMFindUser = *finduser;
				AfxGetMainWnd()->PostMessage(MSG_CMD_GM_GETUSER, WPARAM(&m_GMFindUser), 0);
			}
			else AfxMessageBox("不存在该用户!");
			break;
		}
	case ASS_GM_MESSAGE_OPERATE_ACT:
		{
			if (pNetHead->bHandleCode == ERR_GM_OPERATOR_SUCCESS) 
				AfxMessageBox("操作成功!");
			else 
				AfxMessageBox("操作失败!");
			break;
		}
	case ASS_GM_MESSAGE_PROP_INFO:
		{
			if (!m_GetAwardListing) break;
			int nCount = uDataSize / sizeof(GM_GetPropInfo_t);
			if (nCount > 0)
			{
				GM_GetPropInfo_t * pProp = (GM_GetPropInfo_t *)pNetData;
				for (int i = 0; i < nCount; i++)
				{
					if (m_PropList.count(pProp->iPropID) == 0)
					{
						GM_GetPropInfo_t * p = new GM_GetPropInfo_t;
						*p = *pProp;
						delete m_PropList[p->iPropID];
						m_PropList[p->iPropID] = p;
					}
					pProp++;
				}
			}
			if (pNetHead->bHandleCode == HC_GM_GET_LIST_END)
				m_GetAwardListing = false;
			if (!m_GetAwardTypeing && !m_GetAwardListing) GetGameList();
			break;
		}

	}
	return true;
}

bool CZClientSocket::OnTalkMsg(NetMessageHead* pNetHead, void* pNetData, UINT uDataSize, CTCPClientSocket* pClientSocket)
{
	switch (pNetHead->bAssistantID)
	{
		case ASS_GP_TALK_2_GM:				///< 发起聊天
		case ASS_GP_MSG_FROM_GM:
			{
				GM_Broadcast_t* pMsg = (GM_Broadcast_t *)pNetData;
				m_GMTalkMsg = *pMsg;
				bool *issystem = new bool;
				*issystem = pNetHead->bHandleCode != 0;
				AfxGetMainWnd()->PostMessage(MSG_CMD_GM_TALK, WPARAM(&m_GMTalkMsg), LPARAM(issystem));
				break;	
			}
	}
	return true;
}

bool CZClientSocket::GetGameList()
{
	m_ClientSocket.SendData(MDM_GP_LIST, ASS_GP_LIST_KIND, 0);
	return true;
}

bool CZClientSocket::GetGameRoom(UINT uKindID, UINT uNameID)
{
	MSG_GP_SR_GetRoomStruct GetList;
	GetList.uKindID = uKindID;
	GetList.uNameID = uNameID;
	m_ClientSocket.SendData(&GetList, sizeof(GetList), MDM_GP_LIST, ASS_GP_LIST_ROOM, 0);
	return true;
}

bool CZClientSocket::SendOperate(GM_OperateByID_t *Operate)
{
	Operate->iGMID = m_LogonInfo.dwUserID;
	m_ClientSocket.SendData(Operate, sizeof(GM_OperateByID_t), MDM_GAMEMASTER_MESSAGE, ASS_GM_MESSAGE_OPERATE_ACT, 0);
	return true;
}

bool CZClientSocket::SendFindUser(GM_GetOperationByPlayer_t *play)
{
	m_ClientSocket.SendData(play, sizeof(GM_GetOperationByPlayer_t), MDM_GAMEMASTER_MESSAGE, ASS_GM_MESSAGE_OPERATE_HISTORY, 0);
	return true;
}

bool CZClientSocket::SendTalk(GM_Broadcast_t *talk)
{
	talk->iGMID = m_LogonInfo.dwUserID;
	int ret = m_ClientSocket.SendData(talk, sizeof(GM_Broadcast_t), MDM_GAMEMASTER_MESSAGE, ASS_GM_MESSAGE_BROADCAST, 0);
	XAFCLog::LOG("GM_Socket_Z","");
	XAFCLog::LOG("GM_Socket_Z","+talk addr = 0x%08x size = %d SendData ret = %d",talk,sizeof(GM_Broadcast_t),ret);
	XAFCLog::LOG("GM_Socket_Z","+---- msg = %s",talk->szMsg);
	return true;
}

CString CZClientSocket::innerGetMac()
{
	CString retMac="";
	PIP_ADAPTER_INFO pAdapterInfo;
	PIP_ADAPTER_INFO pAdapter=NULL;
	DWORD dwRetVal=0;
	pAdapterInfo=(IP_ADAPTER_INFO*)malloc(sizeof(IP_ADAPTER_INFO));
	ULONG ulOutBufLen=sizeof(IP_ADAPTER_INFO);
	if(GetAdaptersInfo(pAdapterInfo,&ulOutBufLen)!=ERROR_SUCCESS)
	{
		free(pAdapterInfo);
		pAdapterInfo=(IP_ADAPTER_INFO*)malloc(ulOutBufLen);
	}
	if((dwRetVal=GetAdaptersInfo(pAdapterInfo,&ulOutBufLen))==NO_ERROR)
	{
		pAdapter=pAdapterInfo;
		CString temp;
		while(pAdapter)
		{
			if(pAdapter->Type==6)//pAdapter->Description中包含"PCI"为：物理网卡//pAdapter->Type是71为：无线网卡
			{
				for(UINT i=0;i<pAdapter->AddressLength;i++)
				{
					temp.Format("%02X",pAdapter->Address[i]);
					retMac+=temp;
					//	printf("%02X%c",pAdapter->Address[i],i==pAdapter->AddressLength-1?’n’:’-’);
				}
				break;
			}
			pAdapter=pAdapter->Next;
		}
	}
	if(pAdapterInfo)
		free(pAdapterInfo);
	return retMac;
}

CString CZClientSocket::coreGetCode()
{
	CString strRet="";
	CString str=innerGetMac(),s;
	if(str=="")
	{
		return "";
	}

	int r=0;
	int l=str.GetLength();
	for(int i=0;i<l;i+=2)
	{
		int r1=0,r2=0;
		s=str.Mid(i,1);
		if(s>="0" && s<="9")
			r1=atoi(s);
		if(s=="A" || s=="a")	r1=10;
		if(s=="B" || s=="b")	r1=11;
		if(s=="C" || s=="c")	r1=12;
		if(s=="D" || s=="d")	r1=13;
		if(s=="E" || s=="e")	r1=14;
		if(s=="F" || s=="f")	r1=15;
		s=str.Mid(i+1,1);
		if(s>="0" && s<="9")
			r2=atoi(s);
		if(s=="A" || s=="a")	r2=10;
		if(s=="B" || s=="b")	r2=11;
		if(s=="C" || s=="c")	r2=12;
		if(s=="D" || s=="d")	r2=13;
		if(s=="E" || s=="e")	r2=14;
		if(s=="F" || s=="f")	r2=15;

		CString t;
		r+=r1*16+r2;
		srand(r);
		t.Format("%s%04X%s-",str.Mid(l-i-1,1),rand(),str.Mid(l-i-2,1));
		strRet+=t;
	}
	if(strRet.Right(1)=="-")strRet=strRet.Left(strRet.GetLength()-1);
	return strRet;
}

bool CZClientSocket::Longo()
{
	int rNum = 0;
	CString TML_SN = "";
	CString MatchineCode = "";// coreGetCode();	//取得本机机器码 zxj 2009-11-12 锁定机器

	CString allword = "0123456789qwertyuiopasdfghjklzxcvbnmQWERTYUIOPASDFGHJKLZXCVBNM";
	for (int i = 0; i <= 120; i++)
	{
		rNum = (rand() * 1000) % 62;
		TML_SN += allword[rNum];
	}



	char	m_szMD5Pass[50];
	if(m_ServerInfo.m_nEncryptType == 1)
	{
		MD5_CTX Md5;
		Md5.MD5Update((unsigned char *)m_PassWord.GetBuffer(), lstrlen(m_PassWord));
		unsigned char szMDTemp[16];
		Md5.MD5Final(szMDTemp);
		for (int i=0;i<16;i++) wsprintf(&m_szMD5Pass[i * 2], "%02x", szMDTemp[i]);
	}
	else if(m_ServerInfo.m_nEncryptType == 2)
	{
		unsigned szMDTemp[5];
		CString s = m_PassWord;//"112233";
		SHA1 sha;
		sha.Reset();
		sha<<s;
		sha.Result(szMDTemp);
		for (int i=0;i<5;i++) 
			wsprintf(&m_szMD5Pass[i*8], "%08x", szMDTemp[i]);
	}

	memset(&m_LogonByName, 0, sizeof(m_LogonByName));	
	m_LogonByName.uRoomVer = GAME_PLACE_MAX_VER;//结构的版本
	lstrcpy(m_LogonByName.TML_SN, TML_SN);
	lstrcpy(m_LogonByName.szName, m_UerseName);//结构的用户名
	lstrcpy(m_LogonByName.szMD5Pass, m_szMD5Pass);//结构的密码
	m_LogonByName.gsqPs = 5471;
	lstrcpy(m_LogonByName.szMathineCode, MatchineCode);	//锁定机器，把本机机器码传递到登录数据包里 zxj 2009-11-12


	lstrcpy(m_GMLogin.szMathineCode, MatchineCode);
	lstrcpy(m_GMLogin.szName, m_UerseName);
	lstrcpy(m_GMLogin.szMD5Pass, m_szMD5Pass);
	m_GMLogin.uRoomVer = GAME_PLACE_MAX_VER;

	//m_ClientSocket.SendData(&m_LogonByName, sizeof(m_LogonByName), MDM_GP_LOGON, ASS_GP_LOGON_BY_NAME, 0);
	m_ClientSocket.SendData(&m_GMLogin, sizeof(m_GMLogin), MDM_GAMEMASTER_MESSAGE, ASS_GM_MESSAGE_GM_LOGON, 0);
	return true;
}


