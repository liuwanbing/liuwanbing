#include "StdAfx.h"
#include "WClientSocket.h"


CWClientSocket::CWClientSocket():
m_ClientSocket(this)
{
}

CWClientSocket::CWClientSocket(GM_MSG_O_LogonResult_t* plogonInfo, ComRoomInfo* pRoomInfo): 
m_pLogonInfo(plogonInfo),
m_pRoomInfo(pRoomInfo),
m_ClientSocket(this)
{
}

CWClientSocket::~CWClientSocket(void)
{
	m_ClientSocket.CloseSocket(false);
	m_UserManage.CleanUserInfo();
}




///网络读取消息回调函数
bool CWClientSocket::OnSocketReadEvent(NetMessageHead* pNetHead, void* pNetData, UINT uDataSize, CTCPClientSocket* pClientSocket)
{
	switch (pNetHead->bMainID)
	{
	case MDM_CONNECT:	///连接消息 
		{
			return OnConnect(pNetHead, pNetData, uDataSize, pClientSocket);
		}
	case MDM_GR_USER_LIST:
		{
			return OnGetUserList(pNetHead, pNetData, uDataSize, pClientSocket);
		}
	case MDM_GR_LOGON:
		{
			switch (pNetHead->bAssistantID)
			{
			case ASS_GR_LOGON_SUCCESS:
				{
					break;
				}
			case ASS_GR_LOGON_ERROR:
				{
					pClientSocket->CloseSocket(false);
					if (0 <= pNetHead->bAssistantID && pNetHead->bAssistantID <= 13)
						AfxMessageBox(LOGIN_ERR_STRING[pNetHead->bHandleCode]);
					else if (pNetHead->bAssistantID == 160 || pNetHead->bAssistantID == 161)
						AfxMessageBox(LOGIN_ERR_STRING[13 + pNetHead->bHandleCode - 160 + 1]);
					else 
						AfxMessageBox("未知错误!");
					break;
				}
			}
			break;
		}
	case MDM_GR_USER_ACTION:
		{
			return OnUserAction(pNetHead, pNetData, uDataSize, pClientSocket);
		}
	case MDM_GR_ROOM:
		{
			return OnRoomMsg(pNetHead, pNetData, uDataSize, pClientSocket);
		}
	case MDM_GR_DESK:
		{
			return OnLockMsg(pNetHead, pNetData, uDataSize, pClientSocket);
		}
	}
	return true;
}

///网络连接消息回调函数
bool CWClientSocket::OnSocketConnectEvent(UINT uErrorCode, CTCPClientSocket* pClientSocket)
{
	if (uErrorCode!=0)	//连接出现错误
	{
		AfxMessageBox("连接W服务器失败!");
		return true;
	}
	return true;//连接成功
}

///网络关闭消息回调函数
bool CWClientSocket::OnSocketCloseEvent()
{
	AfxMessageBox("与房间断开连接，请重新登录!");
	return true;
}

///连接成功的回调函数
bool CWClientSocket::OnConnect(NetMessageHead* pNetHead, void* pNetData, UINT uDataSize, CTCPClientSocket* pClientSocket)
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

///获取游戏列表的回调函数
bool CWClientSocket::OnGetUserList(NetMessageHead* pNetHead, void* pNetData, UINT uDataSize, CTCPClientSocket* pClientSocket)
{
	switch (pNetHead->bAssistantID)
	{
	case ASS_GR_ONLINE_USER:
	case ASS_GR_NETCUT_USER:	//用户列表
		{
			UINT uUserCount=uDataSize / sizeof(UserInfoStruct);
			if (uUserCount > 0)
			{
				UserItemStruct * pUserItem=NULL;
				UserInfoStruct * pUserInfo=(UserInfoStruct *)pNetData;
				for (UINT i = 0; i < uUserCount; i++)
				{
					UserInfoStruct * pUserInfoTemp;
					pUserInfoTemp = pUserInfo+i;
					pUserItem = m_UserManage.AddGameUser(pUserInfoTemp, GetUserSortIndex(pUserInfo+i), GetUserConnection((pUserInfo+i)->dwUserID));
				}
				PostMessage(m_hwnd, MSG_CMD_SHOW_GAMEROOM, 0, 0);
			}
			break;
		}
	case ASS_GR_DESK_STATION:	//桌子状态 
		{
			//效验数据
			if (uDataSize!=sizeof(MSG_GR_DeskStation)) return false;
			MSG_GR_DeskStation * pDeskStation=(MSG_GR_DeskStation *)pNetData;
			m_DeskStation.vVirtuals.clear();
			m_DeskStation.vLocks.clear();
			m_DeskStation.vPlayings.clear();
			for (UINT i = 0; i < pNetHead->bHandleCode; i++)
			{
				if ((pDeskStation->bVirtualDesk[i/8]&(1<<(i%8))) != 0) m_DeskStation.vVirtuals.push_back(i);   
				if ((pDeskStation->bDeskLock[i/8]&(1<<(i%8))) != 0) m_DeskStation.vLocks.push_back(i);   
				if ((pDeskStation->bDeskStation[i/8]&(1<<(i%8))) != 0) m_DeskStation.vPlayings.push_back(i);
			}
			PostMessage(m_hwnd, MSG_CMD_SHOW_GAMEDESKSTATE, WPARAM(&m_DeskStation), 0);
		}

	}
	return true;
}



///用户动作回调函数
bool CWClientSocket::OnUserAction(NetMessageHead* pNetHead, void* pNetData, UINT uDataSize, CTCPClientSocket* pClientSocket)
{
	//状态判断
	//if (m_bLogonFinish==false) return true;

	switch (pNetHead->bAssistantID)
	{
	case ASS_GR_USER_HIT_BEGIN:
		{
			MSG_GM_WatchSet* pUserID = (MSG_GM_WatchSet*) pNetData;
			UserItemStruct * pUserItem = m_UserManage.FindOnLineUser(pUserID->dwUserID);
			pUserItem->GameUserInfo.bUserState = USER_DESK_AGREE;
			PostMessage(m_hwnd, MSG_CMD_SHOW_GAMEROOM, 1, pUserID->dwUserID);
			break;
		}
	case ASS_GR_USER_UP:		//用户起来
	case ASS_GR_WATCH_UP:		//旁观起来
		{
			if (uDataSize != sizeof(MSG_GR_R_UserSit)) return false;
			MSG_GR_R_UserSit * pUserUp=(MSG_GR_R_UserSit *)pNetData;
			UserItemStruct * pUserItem = m_UserManage.FindOnLineUser(pUserUp->dwUserID);
			if (pUserItem != NULL)
			{
				pUserItem->GameUserInfo.bDeskNO = 255;
				pUserItem->GameUserInfo.bDeskStation = 255;
				pUserItem->GameUserInfo.bUserState = pUserUp->bUserState;
				PostMessage(m_hwnd, MSG_CMD_SHOW_GAMEROOM, 1, pUserUp->dwUserID);
			}
			break;
		}
	case ASS_GR_USER_SIT:		//用户坐下
	case ASS_GR_WATCH_SIT:		//旁观坐下
	case ASS_GR_QUEUE_USER_SIT:	//排队机中的用户坐下
		{
			if (uDataSize != sizeof(MSG_GR_R_UserSit)) return false;
			MSG_GR_R_UserSit* pUserSit = (MSG_GR_R_UserSit*)pNetData;
			UserItemStruct* pUserItem = m_UserManage.FindOnLineUser(pUserSit->dwUserID);
			if (pUserSit != NULL)
			{
				pUserItem->GameUserInfo.bDeskNO = pUserSit->bDeskIndex;
				pUserItem->GameUserInfo.bDeskStation = pUserSit->bDeskStation;
				pUserItem->GameUserInfo.bUserState = pUserSit->bUserState;
				PostMessage(m_hwnd, MSG_CMD_SHOW_GAMEROOM, 1, pUserSit->dwUserID);
			}
			break;
		}
	case ASS_GR_USER_COME:		//用户进入
		{
			if (uDataSize!=sizeof(MSG_GR_R_UserCome)) return false;
			MSG_GR_R_UserCome * pUserCome = (MSG_GR_R_UserCome *)pNetData;
			UserItemStruct * pUserItem = m_UserManage.AddGameUser(&pUserCome->pUserInfoStruct, GetUserSortIndex(&pUserCome->pUserInfoStruct),
				GetUserConnection(pUserCome->pUserInfoStruct.dwUserID));
			PostMessage(m_hwnd, MSG_CMD_SHOW_GAMEROOM, 2, pUserCome->pUserInfoStruct.dwUserID);
			break;
		}
	case ASS_GR_USER_LEFT:		//用户离开
		{
			if (uDataSize!=sizeof(MSG_GR_R_UserLeft)) return false;
			MSG_GR_R_UserLeft* pUserLeft = (MSG_GR_R_UserLeft*)pNetData;
			UserItemStruct * pUserItem = m_UserManage.FindOnLineUser(pUserLeft->dwUserID);
			int _Index = m_UserManage.GetIndexByUserID(pUserLeft->dwUserID);
			if (pUserItem!=NULL)
			{
				m_UserManage.DeleteGameUser(pUserItem);
				PostMessage(m_hwnd, MSG_CMD_SHOW_GAMEROOM, 3, _Index);
			}
			break;
		}
	case ASS_GR_USER_CUT:		//用户断线
		{
			if (uDataSize != sizeof(MSG_GR_R_UserCut)) return false;
			MSG_GR_R_UserCut * pUserCut = (MSG_GR_R_UserCut *)pNetData;
			UserItemStruct * pUserItem = m_UserManage.FindOnLineUser(pUserCut->dwUserID);
			if (pUserItem!=NULL)
			{
				pUserItem->GameUserInfo.bUserState = USER_CUT_GAME;
				PostMessage(m_hwnd, MSG_CMD_SHOW_GAMEROOM, 1, pUserCut->dwUserID);
			}
			break;
		}

	}
	return true;
}


///房间消息回调函数
bool CWClientSocket::OnRoomMsg(NetMessageHead* pNetHead, void* pNetData, UINT uDataSize, CTCPClientSocket* pClientSocket)
{
	switch (pNetHead->bAssistantID)
	{
	case ASS_GR_USER_AGREE:		///用户同意
		{
			if (uDataSize!=sizeof(MSG_GR_R_UserAgree)) return false;
			MSG_GR_R_UserAgree * pAgreeGame=(MSG_GR_R_UserAgree *)pNetData;

			UserItemStruct * pUserItem = m_UserManage.FindOnLineUser(pAgreeGame->bDeskNO, pAgreeGame->bDeskStation);
			if (pUserItem!=NULL)
			{
				pUserItem->GameUserInfo.bUserState = USER_ARGEE;
				PostMessage(m_hwnd, MSG_CMD_SHOW_GAMEROOM, 1, pUserItem->GameUserInfo.dwUserID);
			}
			break;
		}
	case ASS_GR_GAME_BEGIN:		///游戏开始
		{
			if (uDataSize!=0) return false;
			BYTE bDeskIndex = pNetHead->bHandleCode;
			for (UINT i = 0; i < m_pRoomInfo->uDeskPeople; i++)
			{
				UserItemStruct * pUserItem= m_UserManage.FindOnLineUser(bDeskIndex, i);
				if (pUserItem!=NULL) 
				{
					pUserItem->GameUserInfo.bUserState = USER_PLAY_GAME;
					PostMessage(m_hwnd, MSG_CMD_SHOW_GAMEROOM, 1, pUserItem->GameUserInfo.dwUserID);
				}
			}
			m_DeskStation.vPlayings.push_back(bDeskIndex);
			break;
		}
	case ASS_GR_GAME_END:		///游戏结束
		{
			if (uDataSize!=0) return false;
			MSG_GR_R_UserAgree * pAgreeGame=(MSG_GR_R_UserAgree *)pNetData;

			BYTE bDeskIndex = pNetHead->bHandleCode;
			for (UINT i = 0; i < m_pRoomInfo->uDeskPeople; i++)
			{
				UserItemStruct * pUserItem= m_UserManage.FindOnLineUser(bDeskIndex, i);
				if (pUserItem!=NULL && pUserItem->GameUserInfo.bUserState != USER_CUT_GAME) 
				{
					pUserItem->GameUserInfo.bUserState = USER_SITTING;
					PostMessage(m_hwnd, MSG_CMD_SHOW_GAMEROOM, 1, pUserItem->GameUserInfo.dwUserID);
				}
			}
			for (vector<BYTE>::iterator i = m_DeskStation.vPlayings.begin(); i != m_DeskStation.vPlayings.end();)
			{
				if (*i == bDeskIndex) i = m_DeskStation.vPlayings.erase(i);
				else i++;
			}
			break;
		}
	}
	return true;
}


///封桌
bool CWClientSocket::OnLockMsg(NetMessageHead* pNetHead, void* pNetData, UINT uDataSize, CTCPClientSocket* pClientSocket)
{
	//MSG_GR_SR_MatchDelete* pDeskAlone=(MSG_GR_SR_MatchDelete *)pNetData;
	if (pNetHead->bHandleCode == 1)
	{
		m_DeskStation.vVirtuals.push_back(pNetHead->bAssistantID);
	}
	else if (pNetHead->bHandleCode == 2)
	{
		for (vector<BYTE>::iterator i = m_DeskStation.vVirtuals.begin(); i != m_DeskStation.vVirtuals.end();)
		{
			if ((*i) == pNetHead->bAssistantID) i = m_DeskStation.vVirtuals.erase(i);
			else i++;
		}
	}
	PostMessage(m_hwnd, MSG_CMD_SHOW_GAMEDESKSTATE, 0, 0);
	return true;
}


///获取用户序号
UINT CWClientSocket::GetUserSortIndex(UserInfoStruct * pUserItem)
{
	//if (pUserItem->dwUserID == m_pLogonInfo->dwUserID) return SK_ME_USER;
	if (pUserItem->bGameMaster!=0) return SK_MASTER_USER;
	if (pUserItem->bMember==1) return SK_NORMAL_MEMBER;
	if (pUserItem->bMember==2) return SK_HIGHT_MEMBER;
	if (pUserItem->bMember==3) return SK_HIGHT_MEMBER;	                   
	if (pUserItem->bMember==4) return SK_SPECIAL_MEMBER;				
	if (pUserItem->bMember==5) return SK_FOREVER_MEMBER;					
	if (pUserItem->bMember==6) return SK_SENIOR_MEMBER;
	return SK_NORMAL_USER;
}

UINT CWClientSocket::GetUserConnection(long int dwUserID)
{
	return CM_NORMAL;
}


///连接到服务器
bool CWClientSocket::ConnectToServer()
{
	m_UserManage.CleanUserInfo();
	return m_ClientSocket.Connect(m_pRoomInfo->szServiceIP, m_pRoomInfo->uServicePort);
}

///关闭连接
bool CWClientSocket::CloseSocket()
{
	m_UserManage.CleanUserInfo();
	return m_ClientSocket.CloseSocket(false);
}

///登录
bool CWClientSocket::Longo()
{
	MSG_GR_S_RoomLogon RoomLogon;
	RoomLogon.uGameVer = 0;//m_pRoomInfo->//8;//m_pRoomInfo->dwGameLSVer;//m_GameInfo.dwGameLSVer;
	RoomLogon.uRoomVer = GAME_PLACE_MAX_VER;
	RoomLogon.uNameID = m_pRoomInfo->uNameID;
	RoomLogon.dwUserID =  m_pLogonInfo->dwUserID;
	lstrcpy(RoomLogon.szMD5Pass,m_pLogonInfo->szMD5Pass);

	return m_ClientSocket.SendData(&RoomLogon,sizeof(RoomLogon), MDM_GR_LOGON, ASS_GR_LOGON_BY_ID, 0);
}


///发送操作
bool CWClientSocket::SendOpera(GM_OperateByID_t* opera)
{
	return m_ClientSocket.SendData(opera, sizeof(GM_OperateByID_t), MDM_GAMEMASTER_MESSAGE, ASS_GM_MESSAGE_OPERATE_ACT, 0);
}

///封桌
bool CWClientSocket::LockDesk(int UserID, int iDeskIndex, int state)
{
	MSG_GR_SR_MatchDelete aloneDesk;
	aloneDesk.dwUserID = UserID;
	aloneDesk.bDeskIndex= iDeskIndex;
	aloneDesk.bDeskStation = state;
	m_ClientSocket.SendData(&aloneDesk,sizeof(aloneDesk),MDM_GR_MANAGE,
		(state==1) ? ASS_GR_ALONE_DESK : ASS_GR_DEALONE_DESK, 0);
	return true;
}

///批量封桌
bool CWClientSocket::LockDesk(int UserID, int lower, int upper, int state)
{
	MSG_GR_SR_MatchDelete aloneDesk;
	aloneDesk.dwUserID = UserID;
	for (int i = lower - 1; i < upper; i++)
	{
		bool flag;
		if (state == 1)
		{
			flag = false;
			for (int j = 0; j < m_DeskStation.vVirtuals.size(); j++)
			{

				if (i == m_DeskStation.vVirtuals[j]) flag = true;
			}
			for (int j = 0; j < m_UserManage.GetOnLineUserCount(); j++)
			{

				if (i == m_UserManage.FindOnLineUserByIndex(j)->GameUserInfo.bDeskNO) flag = true;
			}
		}
		else
		{
			flag = true;
			for (int j = 0; j < m_DeskStation.vVirtuals.size(); j++)
			{

				if (i == m_DeskStation.vVirtuals[j]) flag = false;
			}
		}
		if (!flag)
		{
			aloneDesk.bDeskIndex = i;
			aloneDesk.bDeskStation = state;
			m_ClientSocket.SendData(&aloneDesk,sizeof(aloneDesk),MDM_GR_MANAGE,
				(state==1) ? ASS_GR_ALONE_DESK : ASS_GR_DEALONE_DESK, 0);
		}
	}
	return true;
}

///打印日志
bool CWClientSocket::SendPrintLogCmd(GM_MSG_PrintLog_t* pCmd)
{
	return m_ClientSocket.SendData(pCmd, sizeof(GM_MSG_PrintLog_t), MDM_GAMEMASTER_MESSAGE, ASS_GM_MESSAGE_PRINTLOG, 0);
}