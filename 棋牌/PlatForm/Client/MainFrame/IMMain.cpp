// IMMain.cpp : 实现文件
//
#include "stdafx.h"
#include "IMMain.h"
#include "MainRoomEx.h"
#include "IMSetUser.h"


#ifdef  MAINROOM_USE_SHORTMSG
int		CIMMain::m_iMaxShortWnd=10;					//最多窗口
#endif

// CIMMain 对话框

IMPLEMENT_DYNAMIC(CIMMain, CDialog)

CIMMain::CIMMain()
:htiFamilier(NULL)
,htiUnfamilier(NULL)
,htiBlack(NULL)
{
	bInitialed=false;


	InitialIMList();
	
}

CIMMain::~CIMMain()
{
#ifndef  MAINROOM_USE_SHORTMSG
	clearAllSession();
#else
	for (INT_PTR i=0;i<m_ShortMessage.GetCount();i++) 
	{
		delete ((CShortMessage *)m_ShortMessage.GetAt(i));
		m_ShortMessage.SetAt(i,NULL);
	}
#endif
}


BEGIN_MESSAGE_MAP(CIMMain, CTreeCtrl)

	//{{AFX_MSG_MAP(CTreeCtrlEx) 
	ON_WM_PAINT() 
	//}}AFX_MSG_MAP 
	ON_MESSAGE(WM_EXCHANGE_SKIN,OnExchangeSkin)
END_MESSAGE_MAP()


/*
Function		:InitialIMList
Author			:Fred Huang 2008-4-2
Notes			:初始IM窗口
*/

bool CIMMain::InitialIMList(void)
{

	return true;
}

//换肤
LRESULT CIMMain::OnExchangeSkin(WPARAM wpara,LPARAM lpara)
{
#ifndef  MAINROOM_USE_SHORTMSG
	CIMSession *imSession=NULL;
	OutputDebugString("*rende*CIMMain::OnExchangeSkin");
	POSITION pos= imSessionList.GetHeadPosition();
	//先要从队列中找到会话窗口
	while(pos)
	{
		imSession=imSessionList.GetNext(pos);
		if(imSession != NULL)
		{
			//换肤
			imSession->PostMessage(WM_EXCHANGE_SKIN,wpara,0);
		}
	}
#endif
	Invalidate();
	return LRESULT(0);
}

//初始化函数,同时增加根接点
bool CIMMain::Init()
{
	//初始化List Image
	if(m_imgList.m_hImageList)
		m_imgList.DeleteImageList();
	UINT uIconID[]=
	{
		//固定图标
		IDI_FRIEND,
		IDI_UNKNOWN,
		IDI_BLACK,
		IDI_PLAYER
	};
	m_imgList.Create(16,16,ILC_COLOR16,sizeof(uIconID)/sizeof(uIconID[0]),sizeof(uIconID)/sizeof(uIconID[0]));
	SetBkColor(RGB(255,255,255));
	for (UINT i=0;i<sizeof(uIconID)/sizeof(uIconID[0]);i++)
	{
		HICON hIcon=AfxGetApp()->LoadIcon(uIconID[i]);
		m_imgList.Add(hIcon);
		DestroyIcon(hIcon);
	}
	SetImageList(&m_imgList, TVSIL_NORMAL);

	htiFamilier=InsertItem("我的好友",0,0,TVI_ROOT);
	htiUnfamilier=InsertItem("陌生人",1,1,TVI_ROOT);
	htiBlack=InsertItem("黑名单",2,2,TVI_ROOT);
	SetItemData(htiFamilier,1);
	SetItemData(htiUnfamilier,2);
	SetItemData(htiBlack,3);

	Expand(htiFamilier,TVE_EXPAND);
	Expand(htiUnfamilier,TVE_EXPAND);
	Expand(htiBlack,TVE_EXPAND);

	return true;
}

//树控件颜色控制
void   CIMMain::SetItemFont(HTREEITEM   hItem,   LOGFONT&   logfont) 
{ 
	Color_Font   cf; 
	if(   !m_mapColorFont.Lookup(   hItem,   cf   )   ) 
		cf.color   =   (COLORREF)-1; 
	cf.logfont   =   logfont; 
	m_mapColorFont[hItem]   =   cf; 
} 

////////////////////////////////////////////////////////////////////// 
void   CIMMain::SetItemBold(HTREEITEM   hItem,   BOOL   bBold) 
{ 
	SetItemState(hItem,   bBold   ?   TVIS_BOLD:   0,   TVIS_BOLD); 
} 

////////////////////////////////////////////////////////////////////// 
void   CIMMain::SetItemColor(HTREEITEM   hItem,   COLORREF   color) 
{ 
	Color_Font   cf; 
	if(!m_mapColorFont.Lookup(hItem,   cf)) 
		cf.logfont.lfFaceName[0]   =   '\0 '; 
	cf.color   =   color; 
	m_mapColorFont[hItem]   =   cf; 
} 

////////////////////////////////////////////////////////////////////// 
BOOL   CIMMain::GetItemFont(HTREEITEM   hItem,   LOGFONT   *   plogfont) 
{ 
	Color_Font   cf; 
	if(!m_mapColorFont.Lookup(hItem,   cf)) 
		return   FALSE; 
	if(cf.logfont.lfFaceName[0]   ==   '\0 ')   
		return   FALSE; 
	*plogfont   =   cf.logfont; 
	return   TRUE; 

} 

////////////////////////////////////////////////////////////////////// 
BOOL   CIMMain::GetItemBold(HTREEITEM   hItem) 
{ 
	return   GetItemState(hItem,   TVIS_BOLD)   &   TVIS_BOLD; 
} 

////////////////////////////////////////////////////////////////////// 
COLORREF   CIMMain::GetItemColor(HTREEITEM   hItem) 
{ 
	//   Returns   (COLORREF)-1   if   color   was   not   set 
	Color_Font   cf; 
	if(!m_mapColorFont.Lookup(hItem,   cf)) 
		return   (COLORREF)   -   1; 
	return   cf.color; 

}

#ifndef  MAINROOM_USE_SHORTMSG
/*
Function		:openIMSession
Author			:Fred Huang 2008-4-2
Notes			:打开一个会话窗口，先从已经创建的窗口中找，如果没有则创建一个
Parameter		:
dwUserID		:用户的ID号
szName			:用户名称
*/
CIMSession* CIMMain::openIMSession(int dwUserID,CString szName)
{
	CIMSession *imSession=NULL;
	try
	{
		POSITION pos=imSessionList.GetHeadPosition();
		//先要从队列中找到会话窗口
		while(pos)
		{
			imSession=imSessionList.GetNext(pos);
			if(imSession->m_dwUserId==dwUserID)
			{
				imSession->FlashWindow(TRUE);
				return imSession;
			}
		}
		//如队列中没有，则创建一个
		imSession=new CIMSession(this);
		imSession->Create(IDD_IM_SESSION,GetDesktopWindow());
		imSession->setUser(dwUserID,szName,GetMainRoom(),true);
		//设置对话窗口中的父窗口句柄为本窗口，用于发消息
		imSession->setWindowOwner(this->m_hWnd);
		imSession->CenterWindow();
		//加入到队列中
		imSessionList.AddTail(imSession);
		imSession->FlashWindow(TRUE);	
	}
	catch (...)
	{
		TCHAR smsg[1000]={0};
		wsprintf(smsg,"IM_TRY_CATCH : [%s]",__FUNCTION__);
	}

	return imSession;
}
#endif

/*
Function	:OnNetMessage
Notes		:网络消息
Author		:Fred Huang
Date		:2008-04-2
Parameter	:
NetMessageHead	:网络消息头
void *			:数据指针
UINT			:数据包的大小
return		:void
*/
void CIMMain::OnNetMessage(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize)
{
	if(!bInitialed)
		return;
	try
	{
		switch(pNetHead->bAssistantID)
		{
		case ASS_IMS_GETLIST:
			OnNetGetUserList(pNetData);
			break;
		case ASS_IMS_GETUSERINFO:
			OnNetGetUserInfo(pNetData);
			break;
		case ASS_IMS_MSG:
			OnNetGetMsg(pNetData);
			break;
		case ASS_IMS_ADDREQUEST://有人请求加为好友
			OnNetAddRequest(pNetData);
			break;
		case ASS_IMS_ADDREQUESTRESPONSE://有反应了
			OnNetAddResponse(pNetData);
			break;
		case ASS_IMC_ADDREQUEST:
			OnNetAddRetrue(pNetHead, pNetData);
			break;
		case ASS_SET_FRIENDTYPE:
			{
				int _t = *((int*)pNetData);
				GetMainRoom()->m_PlaceUserInfo.iAddFriendType = _t;
				break;
			}
		}
	}
	catch (...)
	{
		TCHAR smsg[1000]={0};
		wsprintf(smsg,"IM_TRY_CATCH : [%s]",__FUNCTION__);
	}

	return;
}

void CIMMain::OnNetAddRetrue(NetMessageHead * pNetHead, void * pData)
{
	if (pNetHead->bHandleCode == 1 || pNetHead->bHandleCode == 2)
	{
		CString strMsg("该用户拒绝任何人加其为好友或者用户现在不在线!");
		bool bGameOpen = false;
		for(int i = 0; i < MAX_GAME_ROOM; i ++)
		{
			if(GetMainRoom()->m_RoomInfo[i].bAccess)
			{
				// 关闭游戏房间
				CGameRoomEx* pGameRoomEx = static_cast<CGameRoomEx*>(GetMainRoom()->m_RoomInfo[i].pGameRoomWnd);
				if (NULL != pGameRoomEx)
				{
					if (pGameRoomEx->m_IGameFrame != NULL && pGameRoomEx->m_IGameFrame->GetGameIsStarting())
					{
						bGameOpen = true;
						pGameRoomEx->InsertSystemMessageWithGame(strMsg);
					}
				}
			}
		}
		if (!bGameOpen)
		{
			DUIOkeyMsgBox(m_hWnd,strMsg);
		}
	}
}

//清空当前用户好友列表信息 --20150516 wzd
void CIMMain::clearUserList()
{
	HTREEITEM hSelectItem = NULL;
	for(int i = 0; i < 3; i++)
	{
		switch(i)
		{
			
			case 0: //好友
			{
				hSelectItem = htiFamilier;
				break;
			}
			case 1: //陌生人
			{
				hSelectItem = htiUnfamilier;
				break;
			}
			case 2: //黑名单
			{
				hSelectItem = htiBlack;
				break;
			}
			
		}
		 if(NULL != hSelectItem)	
		 {
		 	  //获取子项
			  HTREEITEM hChildItem=GetChildItem(hSelectItem); 
			  HTREEITEM hTempItem;
			  //如果有子项
			  while(hChildItem)
			  {
			   hTempItem=hChildItem;
			   //获取下一个子项
			   hChildItem=GetNextSiblingItem(hChildItem);
			   //删除
			   DeleteItem(hTempItem);
		   }
	  }
	}
    //状态位置位，用于重新从数据库获取切换帐号后的用户好友列表信息
    bInitialed = false;
}

/*
Function	:checkUserCount
Notes		:检查本地好友数，并发给服务器进行对比
Author		:Fred Huang
Date		:2008-04-2
Parameter	:void
return		:void
*/
void CIMMain::checkUserCount(void)
{
	if(!bInitialed)
	{
		if(InitialIMList())
		{
			bInitialed=true;
		}
	}
	else
	{
		return;
	}

	//CMainRoomEx *pWnd=(CMainRoomEx*)pParentWnd;
	try
	{
		//填充结构体
		MSG_IM_C_COUNT msg;
		ZeroMemory(&msg,sizeof(MSG_IM_C_COUNT));
		msg.dwUserID=GetMainRoom()->m_PlaceUserInfo.dwUserID;
		msg.dwUserCount=0;//atoi((char*)_bstr_t(vt));
		msg.dwUserCountReturn=0;

		//获取用户好友列表请求
		GetMainRoom()->SendData(&msg,sizeof(msg),MDM_GP_IM,ASS_IMC_GETCOUNT,0);
	}
	catch (...)
	{
		TCHAR smsg[1000]={0};
		wsprintf(smsg,"IM_TRY_CATCH : [%s]",__FUNCTION__);
	}

	return;	
}


/*
Function	:OnNetGetUserList
Notes		:收到服务器返回的好友列表
Author		:Fred Huang
Date		:2008-04-2
Parameter	:pData-服务器返回的数据
return		:void
*/
void CIMMain::OnNetGetUserList(void* pData)
{
	try
	{
		MSG_IM_S_USERLIST *msg=(MSG_IM_S_USERLIST*)pData;
		MSG_IM_S_GETUSERINFO *userInfo=(MSG_IM_S_GETUSERINFO*)msg->sBody;
		for(int i=0;i<msg->nListCount;i++)
		{
			//取每一个人的资料
			addUserToList(userInfo->dwUserID,userInfo->sUserName,userInfo->nSex,userInfo->GroupID,userInfo->nOnline);
			userInfo++;//sizeof(MSG_IM_S_GETUSERINFO);//(MSG_IM_S_GETUSERINFO*)msg->sBody+p;
		}
	}
	catch (...)
	{
		TCHAR smsg[1000]={0};
		wsprintf(smsg,"IM_TRY_CATCH : [%s]",__FUNCTION__);
	}
}
/*
Function	:addUserToList
Notes		:添加用户到好友列表
Author		:Fred Huang
Date		:2008-04-2
Parameter	:
dwUserID	:用户ID
szUserName	:用户姓名，服务器数据库中是Nickname
sex			:性别，男-1，女-0；
GroupID		:组ID，1-好友，2-陌生人，3-黑名单
onLine		:是否在线
return		:void
*/
void CIMMain::addUserToList(int dwUserID, CString szUsername, int sex, int GroupID, int nOnline)
{
	try
	{

		//如果Sex超出范围，取1
		sex=(sex==0)?0:1;
		//如果online超出范围，取1
		nOnline=(nOnline==0)?0:1;

		//陌生人
		HTREEITEM hRoot=htiUnfamilier;
		//好友
		if(GroupID==1)
		{
			hRoot=htiFamilier;
		}
		//黑名单
		if(GroupID==3)
		{
			hRoot=htiBlack;
		}

		//找出用户现在在树中的位置
		HTREEITEM htiUser=FindUserITEM(dwUserID+1000);
		if(htiUser)
		{
			HTREEITEM htiParent = GetParentItem(htiUser);
			DeleteItem(htiUser);
			if(ItemHasChildren(htiParent) == FALSE)
			{
				SetItemImage(htiParent, 0, 0);
			}
		}

		//取得显示的图标索引
		int nImage = 3;
// 		if(sex==0)
// 		{
// 			if(nOnline==0)
// 				nImage=6;
// 			else
// 				nImage=5;
// 		}
// 		else
// 		{
// 			if(nOnline==0)
// 				nImage=4;
// 			else
// 				nImage=3;
// 		}
		//如果是上线，从显示在上面，否则显示在下面		
		if(nOnline)
			htiUser=InsertItem(szUsername,nImage,nImage,hRoot,TVI_FIRST);
		else
			htiUser=InsertItem(szUsername,nImage,nImage,hRoot,TVI_LAST);
		SetItemData(htiUser,dwUserID+1000);
		Expand(GetParentItem(htiUser),TVE_EXPAND);
		//SetItemImage(GetParentItem(htiUser),1,1);

#ifndef  MAINROOM_USE_SHORTMSG
		POSITION pos=imSessionList.GetHeadPosition();
		CIMSession * session=NULL;
		while(pos)
		{
			session=imSessionList.GetNext(pos);
			if(session->m_dwUserId==dwUserID)
			{
				session->setUserOnline(nOnline==0?false:true);
				break;
			}
		}
#endif

	}
	catch (...)
	{
		TCHAR smsg[1000]={0};
		wsprintf(smsg,"IM_TRY_CATCH : [%s]",__FUNCTION__);
	}
}

/*
Function	:FindUserITEM
Notes		:找到用户在树中的位置，需要遍历所有节点
Author		:Fred Huang
Date		:2008-04-2
Parameter	:
dwUserID	:用户ID
return		:
HTREEITEM	:节点
*/
HTREEITEM CIMMain::FindUserITEM(int dwUserID)
{
	try
	{
		HTREEITEM rootHti,hti;
		for(rootHti=GetChildItem(TVI_ROOT);rootHti!=NULL;rootHti=GetNextSiblingItem(rootHti))
		{
			for(hti=GetChildItem(rootHti);hti!=NULL;hti=GetNextSiblingItem(hti))
			{
				if(GetItemData(hti)==dwUserID)
					return hti;
			}
		}
	}
	catch (...)
	{
		TCHAR smsg[1000]={0};
		wsprintf(smsg,"IM_TRY_CATCH : [%s]",__FUNCTION__);
	}
	return NULL;
}


HTREEITEM CIMMain::FindUserITEM(CString szName)
{
	try
	{
		HTREEITEM rootHti,hti;
		for(rootHti=GetChildItem(TVI_ROOT);rootHti!=NULL;rootHti=GetNextSiblingItem(rootHti))
		{
			for(hti=GetChildItem(rootHti);hti!=NULL;hti=GetNextSiblingItem(hti))
			{
				if(GetItemText(hti)== szName)
					return hti;
			}
		}
	}
	catch (...)
	{
		TCHAR smsg[1000]={0};
		wsprintf(smsg,"IM_TRY_CATCH : [%s]",__FUNCTION__);
	}
	return NULL;
}

/*
Function	:OnNetGetUserInfo
Notes		:收到服务器返回的好友信息
Author		:Fred Huang
Date		:2008-04-7
Parameter	:pData-服务器返回的数据
return		:void
*/
void CIMMain::OnNetGetUserInfo(void * pData)
{
	MSG_IM_S_GETUSERINFO *userInfo=(MSG_IM_S_GETUSERINFO*)pData;
	addUserToList(userInfo->dwUserID,userInfo->sUserName,userInfo->nSex,userInfo->GroupID,userInfo->nOnline);
}
/*
Function	:OnNetGetMsg
Notes		:收到服务器转发的好友消息
Author		:Fred Huang
Date		:2008-04-7
Parameter	:pData-服务器返回的数据
return		:void
*/
void CIMMain::OnNetGetMsg(void * pData)
{

	try
	{
		MSG_IM_S_MSG *msg=(MSG_IM_S_MSG*)pData;
		//找到用户在树中的位置
		HTREEITEM hti=FindUserITEM(msg->dwUserID+1000);

		int charType = GetMainRoom()->m_PlaceUserInfo.iAddFriendType / 65535;
		
		if (charType == 1)
		{
			return;
		}
		if (charType == 2 && GetParentItem(hti) != htiFamilier)
		{
			return;
		}

		if(!hti)
		{//如果没有，则加到陌生人中
			hti=InsertItem(msg->szUserName,2,2,htiUnfamilier);
			SetItemData(hti,msg->dwUserID+1000);
		}
		else
		{//如果在黑名单中，直接返回，不响应
			if(GetParentItem(hti)==htiBlack)
				return;
		}
		//打开对话框
		//激活信息窗口
#ifndef  MAINROOM_USE_SHORTMSG
		CIMSession * session=openIMSession(msg->dwUserID,msg->szUserName);		if(!session)return;
		if(!session->m_hWnd)return;
		session->ShowWindow(SW_SHOW);
		session->setUserOnline(true);
		//添加消息
		session->OnGetMessage(msg);
#else
		CShortMessage * pShortMessageWnd=NULL;
		if (msg->dwUserID == GetMainRoom()->m_PlaceUserInfo.dwUserID)
		{
			//自己发送的消息
			pShortMessageWnd=openShortMsg(msg->dwRemoteUserID,msg->szUserName);
			if (pShortMessageWnd!=NULL) 
			{
				pShortMessageWnd->OnRecvMessage(msg->szUserName,msg->szMsg,RGB(0, 0, 255));
			}

		}
		else	//其他游戏者发送信息	
		{
            CString strMsg("");
            strMsg.Format("%s",msg->szMsg);
			pShortMessageWnd = openShortMsg(msg->dwUserID,msg->szUserName);
			if (pShortMessageWnd!=NULL)
			{
				pShortMessageWnd->OnRecvMessage(msg->szUserName,msg->szMsg,RGB(255, 0, 0));
                lstrcpy(msg->szMsg,strMsg);
                pShortMessageWnd->RecordTalkMsg(false,msg);
			}

		}
#endif
	}
	catch (...)
	{
		TCHAR smsg[1000]={0};
		wsprintf(smsg,"IM_TRY_CATCH : [%s]",__FUNCTION__);
	}
}

/*
Function	:SetUser
Notes		:向其它用户要求添加好友
Author		:Fred Huang
Date		:2008-04-7
Parameter	:
dwUser		:用户ID
szName		:用户名
return		:void
*/
void CIMMain::SetUser(int dwUser,CString szName,bool isBlacklist)
{
	try
	{
		HTREEITEM hti;
		if (dwUser == -1 && szName != "")
			hti = FindUserITEM(szName);
		else 
			hti = FindUserITEM(dwUser+1000);

		//已经是好友了
		if(GetParentItem(hti) == htiFamilier)
		{
			CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");

			CString strMsg(fMsg.GetKeyVal("IMDlg","SorryThisGuyIsYourFriend",
				"对不起，该用户已经是您的好友，不需要再添加！"));

			DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,fMsg.GetKeyVal("IMDlg","Tip","提示"),strMsg);

			return;
		}
		//当前用户id和昵称不是自己
		CString strUser("");	
		int iUserTmp = GetMainRoom()->m_PlaceUserInfo.dwUserID;
		strUser.Format("%s",GetMainRoom()->m_PlaceUserInfo.nickName);
		if ((iUserTmp == dwUser)||(strUser == szName))
		{
			DUIMessageBox(m_hWnd,MB_OK|MB_ICONINFORMATION,_T("系统提示"),_T("对不起，不能添加自己为好友！"));
			return ;
		}
		
		if(szName.GetLength()==0)
		{
			szName = GetItemText(hti);
		}

		//添加请求留言
		CIMSetUser imsu;
		imsu.dwUser=dwUser;
		imsu.szName=szName;
		if(isBlacklist == false && imsu.DoModal()==IDCANCEL)
			return;
		MSG_IM_C_ADDREQUEST msg;
		ZeroMemory(&msg,sizeof(MSG_IM_C_ADDREQUEST));

		msg.dwUserID=GetMainRoom()->m_PlaceUserInfo.dwUserID;
		msg.dwRequestedUserID=dwUser;
		memcpy(&msg.sUserName,GetMainRoom()->m_PlaceUserInfo.nickName,sizeof(GetMainRoom()->m_PlaceUserInfo.nickName));
		memcpy(&msg.sRequestedUserName,szName.GetBuffer(), szName.GetLength());
		
		memcpy(&msg.sRequestNotes,imsu.imWords,imsu.imWords.GetLength());
		msg.nMsgLength=imsu.imWords.GetLength();
		msg.cbSize=sizeof(MSG_IM_C_ADDREQUEST)-sizeof(msg.sRequestNotes)+msg.nMsgLength+1;//实际长度

		if (dwUser > -1)
			GetMainRoom()->SendData(&msg,msg.cbSize,MDM_GP_IM,ASS_IMC_ADDREQUEST,0);
		else
			GetMainRoom()->SendData(&msg,msg.cbSize,MDM_GP_IM,ASS_IMC_ADDREQUEST,1);
	}
	catch (...)
	{
		TCHAR smsg[1000]={0};
		wsprintf(smsg,"IM_TRY_CATCH : [%s]",__FUNCTION__);
	}
}

/*
Function	:OnNetAddRequest
Notes		:有人请求加为好友，需要处理
Author		:Fred Huang
Date		:2008-04-7
Parameter	:pData-服务器返回的数据
return		:void
*/
void CIMMain::OnNetAddRequest(void * pData)
{
	try
	{
		MSG_IM_S_ADDREQUEST * msg=(MSG_IM_S_ADDREQUEST*)pData;
		CString stip;
		CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");
		CString str = fMsg.GetKeyVal("IMDlg","MakeFriend","用户 %s 请求加您为好友：\n%s\n您是否同意？");
		stip.Format(str,msg->sUserName,msg->sUserName,msg->sRequestNotes);
		int ret=GetDesktopWindow()->MessageBox(stip,"提示",MB_ICONQUESTION|MB_YESNOCANCEL);//2008-04-28

		//响应
		MSG_IM_C_ADDRESPONSE resp;
		ZeroMemory(&resp,sizeof(MSG_IM_C_ADDRESPONSE));
		resp.dwRequestedUserID=msg->dwRequestedUserID;	//被请求者ID，自己的ID
		if (ret != IDYES)
			resp.dwRefusedUserID = msg->dwUserID;
		resp.dwUserID=msg->dwUserID;					//请求者ID，对方ID

		memcpy(&resp.sRequestedUserName,GetMainRoom()->m_PlaceUserInfo.nickName,sizeof(GetMainRoom()->m_PlaceUserInfo.nickName));
		memcpy(resp.sRequirUsrName,msg->sUserName,sizeof(resp.sRequirUsrName));
		resp.nResponse=ret;
		resp.nSex=0;
		GetMainRoom()->SendData(&resp,sizeof(MSG_IM_C_ADDRESPONSE),MDM_GP_IM,ASS_IMC_ADDREQUESTRESPONSE,0);
	}
	catch (...)
	{
		TCHAR smsg[1000]={0};
		wsprintf(smsg,"IM_TRY_CATCH : [%s]",__FUNCTION__);
	}

}

/*
Function	:OnNetAddResponse
Notes		:请求加为好友，对方有了反应
Author		:Fred Huang
Date		:2008-04-7
Parameter	:pData-服务器返回的数据
return		:void
*/
void CIMMain::OnNetAddResponse(void * pData)
{
	try
	{
		CString strMsg("");
		CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");

		MSG_IM_S_ADDRESPONSE * msg=(MSG_IM_S_ADDRESPONSE*)pData;
		if(msg->nResponse!=IDYES)
		{
			if (msg->dwRefusedUserID == GetMainRoom()->m_PlaceUserInfo.dwUserID)
			{
				strMsg = fMsg.GetKeyVal("IMDlg","RefuseMakeFriend","对不起，对方拒绝您加为好友！");
			}
		}
		else
		{
			HTREEITEM hti=FindUserITEM(msg->dwRequestedUserID);
			if(!hti)
			{
				addUserToList(msg->dwRequestedUserID,msg->sRequestedUserName,1,1,1);

				strMsg = fMsg.GetKeyVal("IMDlg","AcceptMakeFriend","玩家[%s]已经添加到你的好友列表中。");
				if (GetMainRoom()->m_PlaceUserInfo.dwUserID == msg->dwUserID)
					strMsg.Format(strMsg, msg->sRequestedUserName);
				else
					strMsg.Format(strMsg, msg->sRequirUsrName);
			}
		}
		if ("" == strMsg)
			return;

		bool bGameOpen = false;
		for(int i = 0; i < MAX_GAME_ROOM; i ++)
		{
			if(GetMainRoom()->m_RoomInfo[i].bAccess)
			{
				// 关闭游戏房间
				CGameRoomEx* pGameRoomEx = static_cast<CGameRoomEx*>(GetMainRoom()->m_RoomInfo[i].pGameRoomWnd);
				if (NULL != pGameRoomEx)
				{
					if (pGameRoomEx->m_IGameFrame != NULL && pGameRoomEx->m_IGameFrame->GetGameIsStarting())
					{
						bGameOpen = true;
						pGameRoomEx->InsertSystemMessageWithGame(strMsg);
					}
				}
			}
		}
		if (!bGameOpen)
		{
			DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,fMsg.GetKeyVal("IMDlg","Tip","提示"),strMsg);
		}
	}
	catch (...)
	{
		TCHAR smsg[1000]={0};
		wsprintf(smsg,"IM_TRY_CATCH : [%s]",__FUNCTION__);
	}
}

/*
Function	:SetUserGroup
Notes		:重新设置好友组，不需要对方确认
Author		:Fred Huang
Date		:2008-04-7
Parameter	:
dwUserID	:对方的ID号
nGroupID	:组ID，1-好友，3-黑名单,0-删除
return		:void
*/
void CIMMain::SetUserGroup(int dwUserId, char* pNick, int nGroupID)
{
	try
	{
		//向服务器发消息
		MSG_IM_C_SETGROUP msg;
		msg.dwUserID=GetMainRoom()->m_PlaceUserInfo.dwUserID;
		msg.dwRemoteUserID=dwUserId;
		msg.groupID=nGroupID;
		GetMainRoom()->SendData(&msg,sizeof(MSG_IM_C_SETGROUP),MDM_GP_IM,ASS_IMC_SETGROUP,0);
		
		LocalListChange(dwUserId, pNick, nGroupID);
	}
	catch (...)
	{
		TCHAR smsg[1000]={0};
		wsprintf(smsg,"IM_TRY_CATCH : [%s]",__FUNCTION__);
	}

	return;

}

int CIMMain::GetUserGroup(int dwUserId)
{
	HTREEITEM hti=FindUserITEM(dwUserId+1000);
	if(GetParentItem(hti) == htiFamilier)
	{
		return 1;
	}
	else if(GetParentItem(hti) == htiBlack)
	{
		return 3;
	}
	else
	{
		return 2;
	}
	return 0;
}

void CIMMain::LocalListChange(int dwUserId, char* pNick, int nGroupID)
{
	//本地处理
	int nImg = 3;
	CString sname;
	HTREEITEM hti=FindUserITEM(dwUserId + 1000);

	if(pNick == NULL && hti)
	{
		sname=GetItemText(hti);
	}

	if(hti)
	{
		if(nGroupID==0)
		{
			DeleteItem(hti);
			return;
		}
		GetItemImage(hti,nImg,nImg);
		DeleteItem(hti);
	}

	
	else if(pNick == NULL && !hti)
	{
		sname = CString("陌生人");
	}
	else
	{
		sname = CString(pNick);
	}


	HTREEITEM insertAfter=TVI_LAST;
// 	if(nImg==3 || nImg==5)
// 		insertAfter=TVI_FIRST;

	if(nGroupID==1)
	{
		hti=InsertItem(sname,nImg,nImg,htiFamilier,insertAfter);
		Expand(htiFamilier,TVE_EXPAND);
		//SetItemImage(htiFamilier,1,1);
	}
	else if(nGroupID==2)
	{
		hti=InsertItem(sname,nImg,nImg,htiUnfamilier,insertAfter);
		Expand(htiUnfamilier,TVE_EXPAND);
		//SetItemImage(htiBlack,1,1);
	}
	else
	{
		hti=InsertItem(sname,nImg,nImg,htiBlack,insertAfter);
		Expand(htiBlack,TVE_EXPAND);
		//SetItemImage(htiBlack,1,1);
	}
	SetItemData(hti,dwUserId + 1000);
}


void CIMMain::OnPaint()
{
	CPaintDC   dc(this); 

	//   Create   a   memory   DC   compatible   with   the   paint   DC 
	CDC   memDC; 
	memDC.CreateCompatibleDC(&dc); 

	CRect   rcClip,   rcClient; 
	dc.GetClipBox(   &rcClip   ); 
	GetClientRect(&rcClient); 

	//   Select   a   compatible   bitmap   into   the   memory   DC 
	CBitmap   bitmap; 
	bitmap.CreateCompatibleBitmap(   &dc,   rcClient.Width(),   rcClient.Height()   ); 
	memDC.SelectObject(   &bitmap   ); 

	//dc.SetBkMode(TRANSPARENT);

	//   Set   clip   region   to   be   same   as   that   in   paint   DC 
	CRgn   rgn; 
	rgn.CreateRectRgnIndirect(   &rcClip   ); 
	memDC.SelectClipRgn(&rgn); 
	rgn.DeleteObject(); 

	//   First   let   the   control   do   its   default   drawing. 
	CWnd::DefWindowProc(WM_PAINT,   (WPARAM)memDC.m_hDC,   0); 

	HTREEITEM   hItem   =   GetFirstVisibleItem(); 

	int   iItemCount   =   GetVisibleCount()   +   1; 
	while(hItem   &&   iItemCount--) 
	{ 
		CRect   rect; 

		//   Do   not   meddle   with   selected   items   or   drop   highlighted   items 
		UINT   selflag   =   TVIS_DROPHILITED   |   TVIS_SELECTED; 
		Color_Font   cf; 


		if   ((GetItemState(hItem,   selflag)   &   selflag)   
			&&   ::GetFocus()   ==   m_hWnd) 
			; 
		else   if   (m_mapColorFont.Lookup(hItem,   cf)) 
		{ 
			CFont   *pFontDC; 
			CFont   fontDC; 
			LOGFONT   logfont; 

			if(cf.logfont.lfFaceName[0]   !=   '\0 ')   
				logfont   =   cf.logfont; 
			else   { 
				//   No   font   specified,   so   use   window   font 
				CFont   *pFont   =   GetFont();
				pFont-> GetLogFont(   &logfont   ); 
			} 

			if(GetItemBold(hItem)) 
				logfont.lfWeight   =   700; 

			fontDC.CreateFontIndirect(&logfont); 
			pFontDC   =   memDC.SelectObject(&fontDC   ); 

			if(cf.color   !=   (COLORREF)   -   1) 
				memDC.SetTextColor(cf.color); 
			else 
				memDC.SetTextColor(GetSysColor(COLOR_WINDOWTEXT)); 


			CString   sItem   =   GetItemText(hItem); 

			GetItemRect(hItem,   &rect,   TRUE); 
			//memDC.SetBkColor( GetSysColor(COLOR_WINDOW )); 
			memDC.SetBkMode(TRANSPARENT);
			memDC.TextOut(rect.left   +   2,   rect.top   +   5,   sItem); 

			memDC.SelectObject(pFontDC); 
		} 

		hItem   =   GetNextVisibleItem(hItem); 
	} 


	dc.BitBlt(rcClip.left,   rcClip.top,   rcClip.Width(),   rcClip.Height(),   &memDC,   
		rcClip.left,   rcClip.top,   SRCCOPY); 

	memDC.DeleteDC(); 
}

void CIMMain::clearAllSession(void)
{
#ifndef  MAINROOM_USE_SHORTMSG
	try
	{
		CIMSession *imSession=NULL;
		while(imSessionList.GetCount()>0)
		{
			imSession=imSessionList.GetHead();
			imSessionList.RemoveHead();
			delete imSession;
		}
		DeleteAllItems();
		bInitialed=false;
	}
	catch (...)
	{
		TCHAR smsg[1000]={0};
		wsprintf(smsg,"IM_TRY_CATCH : [%s]",__FUNCTION__);
	}
#endif
}

void CIMMain::UpdateAllIcon(void)
{
}

#ifndef  MAINROOM_USE_SHORTMSG
#else
CShortMessage * CIMMain::openShortMsg(long dwUserID, char* pNickName)
{
	CShortMessage * pShortMessageWnd=ActiveShortWnd(dwUserID,pNickName,true);
	if (pShortMessageWnd==NULL)	
	{
		CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");
		
		//AFCMessageBox(fMsg.GetKeyVal("GameRoom","ClosePartWindows","窗口太多,请关闭部分窗口!"));//,MB_ICONQUESTION,this);
	    DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,"系统提示",fMsg.GetKeyVal("GameRoom","ClosePartWindows","窗口太多,请关闭部分窗口!"));
	}

	return pShortMessageWnd;
}

//激活信息窗口
CShortMessage * CIMMain::ActiveShortWnd(long int dwUserID, char* pNickName, bool bCreate)
{
	//变量定义
	INT_PTR iCloseIndex=-1,iHideIndex=-1;
	UserItemStruct stUserItem;

	ZeroMemory(&stUserItem, sizeof(stUserItem));
	stUserItem.GameUserInfo.dwUserID = dwUserID;
	stUserItem.GameUserInfo.dwAccID = 0;
	strcpy(stUserItem.GameUserInfo.nickName, pNickName);

	//寻找敏合窗口
	for (INT_PTR i=0;i<m_ShortMessage.GetCount();i++)
	{
		CShortMessage * pShortMessageWnd=(CShortMessage *)m_ShortMessage.GetAt(i);
		ASSERT(pShortMessageWnd!=NULL);
		if (pShortMessageWnd!= NULL)
		{
			if (dwUserID==pShortMessageWnd->m_dwTargetUserID)
			{
				if (pShortMessageWnd->GetSafeHwnd()==NULL)
				{
					AfxSetResourceHandle(GetModuleHandle(NULL));
					pShortMessageWnd->Create(IDD_SHORT_MESSAGE,GetDesktopWindow());
					pShortMessageWnd->CenterWindow();
				}
				pShortMessageWnd->ShowWindow(SW_SHOWNOACTIVATE);
				return pShortMessageWnd;
			}
		}
		else
		{
			m_ShortMessage.RemoveAt(i);
		}
		if (bCreate==true)
		{
			if ((iCloseIndex==-1)&&(pShortMessageWnd->GetSafeHwnd()==NULL))
				iCloseIndex=i;
			if ((iHideIndex==-1)&&(pShortMessageWnd->GetSafeHwnd()!=NULL)&&(pShortMessageWnd->IsWindowVisible()==FALSE)) iHideIndex=i;
		}
	}

	//使用关闭的窗口
	if ((bCreate==true)&&(iCloseIndex>=0))
	{
		CShortMessage * pShortMessageWnd=(CShortMessage *)m_ShortMessage.GetAt(iCloseIndex);
		ASSERT(pShortMessageWnd!=NULL);
		if (pShortMessageWnd->GetSafeHwnd()==NULL)
		{
			AfxSetResourceHandle(GetModuleHandle(NULL));
			pShortMessageWnd->Create(IDD_SHORT_MESSAGE,GetDesktopWindow());
			pShortMessageWnd->CenterWindow();
		}
		SendMsgUserStruct user;
		pShortMessageWnd->TransUserInfo(&stUserItem, &user);
		pShortMessageWnd->SetTalkInfo(&user,GetMainRoom()->m_PlaceUserInfo.dwGamePower,"大厅");
		pShortMessageWnd->ShowWindow(SW_SHOWNOACTIVATE);
		return pShortMessageWnd;
	}

	//使用隐藏窗口
	if ((bCreate==true)&&(iHideIndex>=0))
	{
		CShortMessage * pShortMessageWnd=(CShortMessage *)m_ShortMessage.GetAt(iHideIndex);
		ASSERT(pShortMessageWnd!=NULL);
		SendMsgUserStruct user;
		pShortMessageWnd->TransUserInfo(&stUserItem, &user);
		pShortMessageWnd->SetTalkInfo(&user,GetMainRoom()->m_PlaceUserInfo.dwGamePower,"大厅");
		pShortMessageWnd->ShowWindow(SW_SHOWNOACTIVATE);
		return pShortMessageWnd;
	}

	//建立新窗口
	if ((bCreate==true)&&(m_ShortMessage.GetCount()<m_iMaxShortWnd))
	{
		try
		{
			CShortMessage * pShortMessageWnd=new CShortMessage(this);
            pShortMessageWnd->SetUserAccount(GetMainRoom()->m_PlaceUserInfo.szName);
			AfxSetResourceHandle(GetModuleHandle(NULL));
			pShortMessageWnd->m_bIsInRoom = false;
			SendMsgUserStruct user;
			pShortMessageWnd->TransUserInfo(&stUserItem, &user);
			pShortMessageWnd->SetTalkInfo(&user,GetMainRoom()->m_PlaceUserInfo.dwGamePower,"大厅");
			pShortMessageWnd->Create(IDD_SHORT_MESSAGE,GetDesktopWindow());
			pShortMessageWnd->CenterWindow();
			pShortMessageWnd->ShowWindow(SW_SHOWNOACTIVATE);
			m_ShortMessage.Add(pShortMessageWnd);
			return pShortMessageWnd;
		}
		catch (...) { return NULL; }
	}

	return NULL;
}

bool CIMMain::GetSendMsgUserStruct(long int lUserID, SendMsgUserStruct * user)
{
	user->bDeskNO = 255;
	user->bGameMaster = 0;
	user->dwUserID = GetMainRoom()->m_PlaceUserInfo.dwUserID;
	user->dwAccID = GetMainRoom()->m_PlaceUserInfo.dwAccID;
	strcpy(user->nickName, GetMainRoom()->m_PlaceUserInfo.nickName);
	return true;
}

SendMsgUserStruct * CIMMain::GetMeUserInfo()
{
	m_SortMsgUserInfo.bDeskNO = 255;
	m_SortMsgUserInfo.bGameMaster = 0;
	m_SortMsgUserInfo.dwUserID = GetMainRoom()->m_PlaceUserInfo.dwUserID;
	m_SortMsgUserInfo.dwAccID = GetMainRoom()->m_PlaceUserInfo.dwAccID;
	strcpy(m_SortMsgUserInfo.nickName, GetMainRoom()->m_PlaceUserInfo.nickName);

	return &m_SortMsgUserInfo;
}

int CIMMain::SendData(void * pData, UINT uBufLen, BYTE bMainID, BYTE bAssistantID, BYTE bHandleCode)
{
	return GetMainRoom()->SendData(pData, uBufLen, bMainID, bAssistantID, bHandleCode);
}


#endif