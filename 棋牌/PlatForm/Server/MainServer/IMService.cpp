#include "StdAfx.h"
#include "IMService.h"
#include "GameLogonManage.h"

extern CString GetAppPath(bool bFource=true);  //yjj 090317

CIMService::~CIMService(void)
{
}
CIMService::CIMService(CWnd *pWnd)
{
	pParentWnd=pWnd;
	CString s = GetAppPath();
	m_TalkFilter.LoadFilterMessage(s.GetBuffer());
};
/*
Function	:OnNetMessage
Notes		:网络消息
Author		:Fred Huang
Date		:2008-04-02
Parameter	:
NetMessageHead	:网络消息头
void *			:数据指针
UINT			:数据包的大小
UING			:未使用，原指IP地址
UINT			:客户在服务器的SOCKET索引
return		:void
*/
void CIMService::OnNetMessage(NetMessageHead * pNetHead, void * pData, UINT uSize, ULONG uAccessIP, UINT uIndex, DWORD dwHandleID)
{
	if(pNetHead->bMainID!=MDM_GP_IM)
		return;
	switch(pNetHead->bAssistantID)
	{
	case ASS_IMC_GETCOUNT:
		OnNetGetUserCount(pData,uSize,uIndex,dwHandleID);			//取好友总数
		break;
	case ASS_IMC_ADDREQUEST:
		OnNetAddRequest(pNetHead, pData,uSize,uIndex,dwHandleID);			//请求加为好友
		break;
	case ASS_IMC_ADDREQUESTRESPONSE:
		OnNetAddResponse(pData,uSize,uIndex,dwHandleID);
		break;
	case ASS_IMC_MSG:
		OnNetUserMessage(pData,uSize,uIndex,dwHandleID);
		break;
	case ASS_IMC_SETGROUP:
		OnNetUserSetGroup(pData,uSize,uIndex,dwHandleID);
		break;
	case ASS_IMS_ADDREQUESTRESPONSE:
	case ASS_IMS_MSG:
	case ASS_IMS_NOAC:
		OnNetAddRequest_Z(pNetHead, pData,uSize,uIndex,dwHandleID);	
		break;
	}
}

/*
Function	:OnNetGetUserCount
Notes		:客户请求取得服务器上的好友数量
Author		:Fred Huang
Date		:2008-04-02
Parameter	:
NetMessageHead	:网络消息头
void *			:数据指针
UINT			:数据包的大小
UING			:未使用，原指IP地址
UINT			:客户在服务器的SOCKET索引
return		:void
*/
void CIMService::OnNetGetUserCount(void * pData, UINT uSize, UINT uIndex, DWORD dwHandleID)
{
	//取管理窗口
	CGameLogonManage *pWnd=(CGameLogonManage*)pParentWnd;
	
	MSG_IM_C_COUNT * msg=(MSG_IM_C_COUNT*)pData;
	//需要从数据库中取得，填充结构
	IM_GP_I_GETUSERCOUNT im_GetUserCount;
	ZeroMemory(&im_GetUserCount,sizeof(IM_GP_I_GETUSERCOUNT));
	im_GetUserCount.dwUserID=msg->dwUserID;
	im_GetUserCount.dwUserCount=msg->dwUserCount;
	
	pWnd->m_SQLDataManage.PushLine(&im_GetUserCount.DataBaseHead,sizeof(IM_GP_I_GETUSERCOUNT),DTK_GP_IM_GETCOUNT,uIndex,0);

}

/*
Function	:OnNetMessage
Notes		:用户发送消息，这里只是直接转发
Author		:Fred Huang
Date		:2008-04-07
Parameter	:
NetMessageHead	:网络消息头
void *			:数据指针
UINT			:数据包的大小
UING			:未使用，原指IP地址
UINT			:客户在服务器的SOCKET索引
return		:void
*/
void CIMService::OnNetUserMessage(void * pData, UINT uSize, UINT uIndex, DWORD dwHandleID)
{
	//取管理窗口
	CGameLogonManage *pWnd=(CGameLogonManage*)pParentWnd;

	MSG_IM_C_MSG *msg=(MSG_IM_C_MSG*)pData;

	if (!pWnd->IsUserAlreadyLogon(msg->dwRemoteUserID))
	{
		pWnd->SendDataByUser(msg->dwRemoteUserID, pData, uSize, MDM_GP_IM, ASS_IMS_MSG, 0, 0);
		return;
	}

	for(int i=0; i<pWnd->m_UserList.GetCount(); i++)
	{
		LPUSER p = (LPUSER)pWnd->m_UserList.GetAt(i);
		if(p == NULL)
			continue;
		if(p->UserID == msg->dwRemoteUserID)
		{
			msg->szMsg[msg->szMsgLength + 1] = 0;
			//AfxMessageBox(msg->szMsg);
			if (!m_TalkFilter.IsAllowableTalk(msg->szMsg,strlen(msg->szMsg)))
			{
				const CHAR* pTemTalk = m_TalkFilter.GetReplaceContent();
				strcpy(msg->szMsg,pTemTalk);
				msg->szMsgLength = strlen(pTemTalk) + 1;
				msg->szMsg[msg->szMsgLength + 1] = 0;
				msg->cbSize=sizeof(MSG_IM_C_MSG)-sizeof(msg->szMsg)+msg->szMsgLength+1;
			}
			pWnd->m_TCPSocket.SendData(p->iSocketIndex,msg,msg->cbSize,MDM_GP_IM,ASS_IMS_MSG,0,0);
			return;
		}
	}	
	//用户不在线，回发数据头，用户不在线，但也可以不操作
	//发回数据头
	//pWnd->m_TCPSocket.SendData(uIndex,msg,msg->cbSize-msg->szMsgLength,MDM_GP_IM,ASS_IMS_USERNOTONLINE,0,dwHandleID);
}

/*
Function	:OnNetAddRequest
Notes		:请求添加好友，这里只是转发
Author		:Fred Huang
Date		:2008-04-07
Parameter	:
NetMessageHead	:网络消息头
void *			:数据指针
UINT			:数据包的大小
UING			:未使用，原指IP地址
UINT			:客户在服务器的SOCKET索引
return		:void
*/
void CIMService::OnNetAddRequest(NetMessageHead * pNetHead, void * pData, UINT uSize, UINT uIndex, DWORD dwHandleID)
{
	//取管理窗口
	CGameLogonManage *pWnd=(CGameLogonManage*)pParentWnd;

	MSG_IM_C_ADDREQUEST *msg=(MSG_IM_C_ADDREQUEST*)pData;

	if (pNetHead->bHandleCode == 0)
	{
		if (!pWnd->IsUserAlreadyLogon(msg->dwRequestedUserID))
		{
			pWnd->SendDataByUser(msg->dwRequestedUserID, pData, uSize, pNetHead->bMainID, pNetHead->bAssistantID, pNetHead->bHandleCode, 0);
			return;
		}
	}
	else
	{
		if (!pWnd->IsUserAlreadyLogonByName(msg->sUserName))
		{
			pWnd->SendDataByUser(msg->dwRequestedUserID, pData, uSize, pNetHead->bMainID, pNetHead->bAssistantID, pNetHead->bHandleCode, 0);
			return;
		}
	}


	if (pNetHead->bHandleCode == 0)
	{
		for(int i=0; i<pWnd->m_UserList.GetCount(); i++)
		{
			LPUSER p = (LPUSER)pWnd->m_UserList.GetAt(i);
			if(p == NULL)
				continue;
			if(p->UserID ==msg->dwRequestedUserID)
			{
				strcpy(msg->sRequestedUserName, p->UserName);
				if (p->iAddFriendType % 65535 == 0)
					pWnd->SendDataByUser(msg->dwRequestedUserID,msg,msg->cbSize,MDM_GP_IM,ASS_IMS_ADDREQUEST,0,0);
				else
					pWnd->SendDataByUser(msg->dwUserID,msg,msg->cbSize,MDM_GP_IM,ASS_IMC_ADDREQUEST,1,0);
				return;
			}
		}	
	}
	else
	{
		for(int i=0; i<pWnd->m_UserList.GetCount(); i++)
		{
			LPUSER p = (LPUSER)pWnd->m_UserList.GetAt(i);
			if(p == NULL)
				continue;
			if(strcmp(p->UserName, msg->sRequestedUserName) == 0)
			{
				msg->dwRequestedUserID = p->UserID;
				if (p->iAddFriendType % 65535 == 0)
					pWnd->SendDataByUser(msg->dwRequestedUserID,msg,msg->cbSize,MDM_GP_IM,ASS_IMS_ADDREQUEST,0,0);
				else
					pWnd->SendDataByUser(msg->dwUserID,msg,msg->cbSize,MDM_GP_IM,ASS_IMC_ADDREQUEST,1,0);
				return;
			}
		}	
	}

	pWnd->m_TCPSocket.SendData(uIndex,msg,msg->cbSize,MDM_GP_IM,ASS_IMC_ADDREQUEST,2,0);

}

/*
Function	:OnNetAddResponse
Notes		:请求添加好友，这里只是转发的回应，当同意时，要写入数据库中，不同意时，直接转发消息即可
Author		:Fred Huang
Date		:2008-04-07
Parameter	:
NetMessageHead	:网络消息头
void *			:数据指针
UINT			:数据包的大小
UING			:未使用，原指IP地址
UINT			:客户在服务器的SOCKET索引
return		:void
*/
void CIMService::OnNetAddResponse(void * pData, UINT uSize, UINT uIndex, DWORD dwHandleID)
{
	//取管理窗口
	CGameLogonManage *pWnd=(CGameLogonManage*)pParentWnd;

	MSG_IM_S_ADDRESPONSE *msg=(MSG_IM_S_ADDRESPONSE*)pData;

	//if (!pWnd->IsUserAlreadyLogon(msg->dwRequestedUserID))
	//{
	//	pWnd->SendDataByUser(msg->dwRequestedUserID, pData, uSize, MDM_GP_IM, ASS_IMS_ADDREQUESTRESPONSE, 0, 0);
	//	return;
	//}


	//给同意的人也发一条游戏记录，添加对方为好友 yjj 090323
	MSG_IM_S_ADDRESPONSE  tempmsg;
	tempmsg.dwRequestedUserID = msg->dwUserID;
	tempmsg.dwUserID = msg->dwRequestedUserID;
	tempmsg.nResponse = msg->nResponse;
	memcpy(tempmsg.sRequestedUserName,msg->sRequirUsrName,sizeof(tempmsg.sRequestedUserName));
	memcpy(tempmsg.sRequirUsrName,msg->sRequestedUserName,sizeof(tempmsg.sRequirUsrName));



	//
	int iCount = 0;

	//直接转发消息给用户

	LPUSER pUser = NULL;
	LPUSER pUserTarget = NULL;

	pUser = pWnd->GetUserByID(msg->dwUserID);
	if (pUser != NULL) 
		pWnd->m_TCPSocket.SendData(pUser->iSocketIndex,msg,sizeof(MSG_IM_S_ADDRESPONSE),MDM_GP_IM,ASS_IMS_ADDREQUESTRESPONSE,0,0);
	else 
		pWnd->SendDataByUser(msg->dwUserID, msg, sizeof(MSG_IM_S_ADDRESPONSE), MDM_GP_IM, ASS_IMS_ADDREQUESTRESPONSE, 0, 0);
	pUserTarget = pWnd->GetUserByID(tempmsg.dwUserID);
	if (pUserTarget != NULL) 
		pWnd->m_TCPSocket.SendData(pUserTarget->iSocketIndex,&tempmsg,sizeof(MSG_IM_S_ADDRESPONSE),MDM_GP_IM,ASS_IMS_ADDREQUESTRESPONSE,0,0);

	if(msg->nResponse==IDYES)
	{
		IM_GP_I_SETUSER imISetUser;
		imISetUser.dwUserID=msg->dwUserID;
		imISetUser.dwRemoteID=msg->dwRequestedUserID;
		imISetUser.groupID=1;//同意加为好友，直接放到好友中

		pWnd->m_SQLDataManage.PushLine(&imISetUser.DataBaseHead,sizeof(IM_GP_I_SETUSER),DTK_GP_IM_SETUSER,uIndex,0);


		//IM_GP_I_SETUSER imISetUser2;
		//imISetUser2.dwUserID=tempmsg.dwUserID;
		//imISetUser2.dwRemoteID=tempmsg.dwRequestedUserID;
		//imISetUser2.groupID=1;//同意加为好友，直接放到好友中

		//pWnd->m_SQLDataManage.PushLine(&imISetUser2.DataBaseHead,sizeof(IM_GP_I_SETUSER),DTK_GP_IM_SETUSER,uIndex,0);
	}
}

/*
Function	:OnNetUserSetGroup
Notes		:用户修改组
Author		:Fred Huang
Date		:2008-04-07
Parameter	:
NetMessageHead	:网络消息头
void *			:数据指针
UINT			:数据包的大小
UING			:未使用，原指IP地址
UINT			:客户在服务器的SOCKET索引
return		:void
*/
void CIMService::OnNetUserSetGroup(void * pData, UINT uSize, UINT uIndex, DWORD dwHandleID)
{
	//取管理窗口
	CGameLogonManage *pWnd=(CGameLogonManage*)pParentWnd;

	MSG_IM_C_SETGROUP *msg=(MSG_IM_C_SETGROUP*)pData;

	//直接修改服务器，不需要返回

	IM_GP_I_SETUSER imISetUser;
	imISetUser.dwUserID=msg->dwUserID;
	imISetUser.dwRemoteID=msg->dwRemoteUserID;
	imISetUser.groupID=msg->groupID;
	pWnd->m_SQLDataManage.PushLine(&imISetUser.DataBaseHead,sizeof(IM_GP_I_SETUSER),DTK_GP_IM_SETUSER,uIndex,0);
#ifdef HWACC
	int a= 8;
#endif
}

void CIMService::OnNetAddRequest_Z(NetMessageHead * pNetHead, void * pData, UINT uSize, UINT uIndex, DWORD dwHandleID)
{
	//取管理窗口
	CGameLogonManage *pWnd=(CGameLogonManage*)pParentWnd;
	pWnd->m_TCPSocket.SendData(uIndex, pData, uSize, pNetHead->bMainID, pNetHead->bAssistantID, pNetHead->bHandleCode, 0);
}