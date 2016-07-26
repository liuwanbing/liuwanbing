#include "StdAfx.h"
#include "BackWSocket.h"



CBackWSocket::CBackWSocket(void):
m_ClientSocket(this)
{
}

CBackWSocket::~CBackWSocket(void)
{
}

///网络连接消息回调函数
bool CBackWSocket::OnConnect(NetMessageHead* pNetHead, void* pNetData, UINT uDataSize, CTCPClientSocket* pClientSocket)
{
	if (pNetHead->bAssistantID == ASS_NET_TEST)	//网络测试信息
	{

		pClientSocket->SendData(MDM_CONNECT, ASS_NET_TEST, 0);//连接消息类型
		return true;
	}
	else if ((pNetHead->bAssistantID == 2) || (pNetHead->bAssistantID == 3))	//连接成功
	{
		MSG_S_ConnectSuccess * _p = (MSG_S_ConnectSuccess *)pNetData;
		if (_p != NULL)
		{
			pClientSocket->SetCheckCode(_p->i64CheckCode, SECRET_KEY);
		}

		return Longo();
	}
}

///网络读取消息回调函数
bool CBackWSocket::OnSocketReadEvent(NetMessageHead* pNetHead, void* pNetData, UINT uDataSize, CTCPClientSocket* pClientSocket)
{
	switch (pNetHead->bMainID)
	{
	case MDM_CONNECT:	//连接消息 
		{
			return OnConnect(pNetHead, pNetData, uDataSize, pClientSocket);
		}
	case MDM_GR_LOGON:
		{
			switch (pNetHead->bAssistantID)
			{
			case ASS_GR_LOGON_SUCCESS:
				{
					PostMessage(m_hwnd, MSG_CMD_BACKWLOGIN, 0, 0);
					break;
				}
			case ASS_GR_LOGON_ERROR:
				{
					pClientSocket->CloseSocket(true);
					AfxMessageBox(ERR_RP_STRING[pNetHead->bHandleCode]);
					break;
				}
			}
			break;
		}
	}
	return true;
}
//网络连接消息回调函数
bool CBackWSocket::OnSocketConnectEvent(UINT uErrorCode, CTCPClientSocket* pClientSocket)
{
	if (uErrorCode!=0)	//连接出现错误
	{
		AfxMessageBox("连接W服务器失败!");
		//PostQuitMessage(0);
		return true;
	}
	return true;//连接成功
}
//网络关闭消息回调函数
bool CBackWSocket::OnSocketCloseEvent()
{
	return true;
}


///连接到服务器
bool CBackWSocket::ConnectToServer()
{
	return m_ClientSocket.Connect(m_pRoomInfo->szServiceIP, m_pRoomInfo->uServicePort);
}

///断开连接
bool CBackWSocket::closesocket()
{
	return m_ClientSocket.CloseSocket(true);
}

///登录
bool CBackWSocket::Longo()
{
	MSG_GR_S_RoomLogon RoomLogon;
	RoomLogon.uGameVer = 8;//m_pRoomInfo->dwGameLSVer;//m_GameInfo.dwGameLSVer;
	RoomLogon.uRoomVer = GAME_PLACE_MAX_VER;
	RoomLogon.uNameID = m_pRoomInfo->uNameID;
	RoomLogon.dwUserID =  m_pLogonInfo->dwUserID;
	lstrcpy(RoomLogon.szMD5Pass,m_pLogonInfo->szMD5Pass);

	return m_ClientSocket.SendData(&RoomLogon,sizeof(RoomLogon), MDM_GR_LOGON, ASS_GR_LOGON_BY_ID, 0);
}

///发送对用户的操作
bool CBackWSocket::SendOpera(GM_OperateByID_t* opera)
{
	opera->iGMID = m_pLogonInfo->dwUserID;
	m_ClientSocket.SendData(opera, sizeof(GM_OperateByID_t), MDM_GAMEMASTER_MESSAGE, ASS_GM_MESSAGE_OPERATE_ACT, 0);
	return m_ClientSocket.CloseSocket();
}