// LoveSendClassForExe.cpp : 实现文件


#include "StdAfx.h"
#include "LoveSendClassForEXE.h"
#include "../PlayerLogic.h"

// CLoveSendClassForEXE

void DebugPrintf(const char *p, ...)
{
	char szFilename[256];
	sprintf( szFilename, "E:\\bzw\\DebugInfo\\DevForExe.txt" );
	va_list arg;
	va_start( arg, p );
	FILE *fp = fopen( szFilename, "a" );
	if (NULL == fp)
	{
		return;
	}
	vfprintf(fp, p, arg );
	fprintf(fp,"\n");
	fclose(fp);
}

IMPLEMENT_DYNAMIC(CLoveSendClassForEXE, CDialog)

CLoveSendClassForEXE::CLoveSendClassForEXE(void)
{
	m_hWndChannel = NULL;
	m_bWatchMode=true;
	m_bWatchOther=false;
}

CLoveSendClassForEXE::~CLoveSendClassForEXE(void)
{
}

BEGIN_MESSAGE_MAP(CLoveSendClassForEXE, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_COPYDATA()
	ON_WM_DESTROY()
	ON_WM_TIMER()
END_MESSAGE_MAP()

// 初始化和退出前与EXE关联
int CLoveSendClassForEXE::OnInitDialog()
{
	// 初始化用来保护消息缓冲队列的临界区
	//InitializeCriticalSection(&m_csForIPCMessage);
	m_lstIPCMessage.clear();
	InitIPC();
	StartGameClient();
	ShowWindow(SW_HIDE);
	return 0;
}
void CLoveSendClassForEXE::OnDestroy()
{
	StopGameClient();
	DestroyIPC();
	// 初始化用来保护消息缓冲队列的临界区
	//DeleteCriticalSection(&m_csForIPCMessage);
}

//============================================================================\\
// ↓向服务器发送消息

//发送函数
int CLoveSendClassForEXE::SendGameData(void * pData, UINT uBufLen, BYTE bMainID, BYTE bAssistantID, BYTE bHandleCode)
{
	return 0;
	if (m_pGameInfo->pMeUserInfo->GameUserInfo.bDeskNO==255) 
		return 0;
	return m_pGameInfo->pISocketSend->SendGameData(pData,uBufLen,bMainID,bAssistantID,bHandleCode);
}
//发送函数
int CLoveSendClassForEXE::SendGameData(BYTE bMainID, BYTE bAssistantID, BYTE bHandleCode)
{
	return 0;
	return m_pGameInfo->pISocketSend->SendGameData(bMainID,bAssistantID,bHandleCode);
}
// ↑向服务器发送消息
//============================================================================//



//----------------------------------------------------------------------------\\
// 从IFrameInterface接口继承的纯虚函数

//初始化
bool CLoveSendClassForEXE::InitFrame(GameInfoStruct * pGameInfo, CWnd * pGameRoom)
{
	m_pGameInfo=pGameInfo;
	TranslateGameInfo();
	CIPCSendCopyData SendCopyData(m_hWndChannel,m_hWnd);
	SendCopyData.SendData(IPC_MAIN_CONFIG, IPC_SUB_SERVER_INFO, &m_GameInfo, sizeof(m_GameInfo));
	m_pGameRoom=pGameRoom;
	m_pGameRoom->SendMessage(IDM_GET_ROOMNAME,0,(LPARAM)&(m_szGameRoomName[0]));
	return true;
}

//建立窗口
bool CLoveSendClassForEXE::AFCCreateFrame()
{
	if (GetSafeHwnd()==NULL) 
	{
		AfxSetResourceHandle(GetModuleHandle(FACE_DLL_NAME));
		Create(IDD_NULL, m_pGameRoom);
		AfxSetResourceHandle(GetModuleHandle(NULL));
	}
	else
	{
		//		if(InitIPC())
		StartGameClient();
	}

	return false;
}
//关闭窗口
bool CLoveSendClassForEXE::AFCCloseFrame()
{
	StopGameClient();
	//DestroyIPC();
	if (GetSafeHwnd()!=NULL) 
	{
		ShowWindow (SW_HIDE);
		//DestroyWindow();
	}
	//CWnd * winApp=AfxGetApp()->m_pMainWnd;
	//if(winApp && winApp->m_hWnd)
	//	winApp->PostMessage(WM_USER+100,3,0);
	return false;
}
//显示窗口
void CLoveSendClassForEXE::AFCShowFrame(int nShow)
{
	// 通知客户端EXE窗口显示
	ShowWindow(SW_HIDE);
	return;
}
//发送windows消息
LRESULT CLoveSendClassForEXE::SendWindowMessage(UINT msg,WPARAM wParam,LPARAM lParam)
{
	//---------此处应当通知客户端EXE窗口
	////购买了道具后，道具更新
	if((msg>WM_USER+150) && (msg<WM_USER+155))
	{
		SendPlayerData(this->m_pGameInfo->pMeUserInfo);
		SendGlbData();
		return 0;
	}
	if (msg==WM_BRING_GAMEWND_TO_TOP)
	{
		SendBringToTopData();
	}

	////在道具面板使用了道具
	//if(msg==WM_USER+152)
	//{
	//	UsePropItem(wParam);
	//	return 0;
	//}
	return SendMessage(msg,wParam,lParam);
}
//删除对象
void CLoveSendClassForEXE::DeleteFrame()
{
	StopGameClient();
	if (GetSafeHwnd() != NULL)
		OnCancel();
	delete this;
}

//控制消息，来自进程内部函数的调用，由CGameRoomEx通过接口指针调用，pControlData里可能带有指针
//必须先把指针内容提取出来再送往EXE进程，否则会出错
UINT CLoveSendClassForEXE::OnControlMessage(UINT uControlCode, void * pControlData, UINT uDataSize)
{
	int nBufferLen = 0;
	BYTE *pBuffer = NULL;
	//处理数据
	switch (uControlCode)
	{
	case CM_USER_SEND_TIMES_MONEY: // 用户获赠金币
		{
			//send to game talking
			// 数据结构里也含指针，不能直接打包发送到EXE进程
			int nBufferLen = sizeof(UINT) + sizeof(CM_UserState_Send_Times_Money_For_Exe);
			BYTE *pBuffer = new BYTE[nBufferLen];

			memcpy( pBuffer, &uControlCode, sizeof(UINT) );
			CM_UserState_Send_Times_Money_For_Exe *pDes = (CM_UserState_Send_Times_Money_For_Exe *)(pBuffer + sizeof(UINT));
			CM_UserState_Send_Times_Money *pSrc = (CM_UserState_Send_Times_Money*)pControlData;
// 			pDes->dwUserID = pSrc->dwUserID;
// 			pDes->iSendMoney = pSrc->dwGetMoney;
// 			pDes->iSendTimes = pSrc->dwMoneyOnTimes;
// 			pDes->iLastTimes = pSrc->dwTimesNeed;
			memcpy(pDes,pSrc,sizeof(CM_UserState_Send_Times_Money));

			FlushMessage();
			CIPCSendCopyData SendCopyData(m_hWndChannel,m_hWnd);
			SendCopyData.SendData(IPC_MAIN_USER, IPC_SUB_USER_SCORE, (void*)pBuffer, (WORD)nBufferLen);

			delete [] pBuffer;
			pBuffer = NULL;
			break;
		}
	case CM_USER_STATE:		//用户状态改变
		{
			// 数据结构里含有指针，必须先把指针内容提取出来，再打包发送到EXE进程
			//效验数据
			if (uDataSize!=sizeof(CM_UserState_Change)) return 1;
			CM_UserState_Change * pStateChange=(CM_UserState_Change *)pControlData;
			// 实际要传的数据是一个结构
			//			CM_UserState_Change_ForExe *pBuffer = new CM_UserState_Change_ForExe;
			int nBufferLen = sizeof(UINT) + sizeof(CM_UserState_Change_ForExe);
			BYTE *pBuffer = new BYTE[nBufferLen];
			CM_UserState_Change_ForExe *p = (CM_UserState_Change_ForExe *)(pBuffer + sizeof(UINT));
			UINT *pControlCode = (UINT*)pBuffer;
			*pControlCode =uControlCode;
			p->bActionCode = pStateChange->bActionCode;
			p->bDeskStation = pStateChange->bDeskStation;

			if (pStateChange->pUserItem != NULL)
			{
				//// 先更新本地数据
				//UserItemStruct *pThisUserItem = m_pGameInfo->pIFindUser->FindOnLineUser(pStateChange->pUserItem->GameUserInfo.dwUserID);
				//if (NULL != pThisUserItem)
				//{
				//	memcpy( pThisUserItem, pStateChange->pUserItem, sizeof(UserItemStruct) );
				//}
				// 再通知EXE游戏程序
				memcpy( &p->uisUserItem, pStateChange->pUserItem, sizeof(UserItemStruct) );
			}
			//////////////////////////////////////////////////////////////////////////
			//add by 曾宪冬

			// 如果是自己的消息
			if (p->uisUserItem.GameUserInfo.dwUserID == m_pGameInfo->pMeUserInfo->GameUserInfo.dwUserID)
			{
				// 自己离桌的消息，不需要缓存
				if ((p->bActionCode == ASS_GR_USER_UP)
					||(p->bActionCode == ASS_GR_WATCH_UP)
					||(p->bActionCode == ASS_GR_USER_LEFT)
					||(p->bActionCode == ASS_GR_USER_CUT)
					)
				{
					delete [] pBuffer;
					pBuffer = NULL;
					break;
				}
			}

			//////////////////////////////////////////////////////////////////////////
			// 如果客户端还没有启动，则应当把这个消息暂存起来
			if (NULL == m_hWndChannel || m_GameInfo.uisMeUserInfo.GameUserInfo.bDeskStation == 255)
			{
				MessageToSendStruct msg;
				msg.ipcMain = IPC_MAIN_USER;
				msg.ipcSub = IPC_SUB_USER_STATUS;
				msg.pBuffer = pBuffer;
				msg.wSize = nBufferLen;

				DebugPrintf("游客户端还没有启动，暂存信息, uControlCode = [%d], p->bActionCode = [%d], msg.pBuffer =[%X], msg.wSize = [%d]",
					*((UINT*)pBuffer),
					p->bActionCode,
					msg.pBuffer,
					msg.wSize
					);
				//EnterCriticalSection(&m_csForIPCMessage);
				m_lstIPCMessage.push_back(msg);
				//LeaveCriticalSection(&m_csForIPCMessage);
			}
			else
			{
				DebugPrintf("游客户端已经启动，发送信息, p->bActionCode = [%d]", p->bActionCode);
				FlushMessage();
				CIPCSendCopyData SendCopyData(m_hWndChannel,m_hWnd);
				SendCopyData.SendData(IPC_MAIN_USER, IPC_SUB_USER_STATUS, (void*)pBuffer, (WORD)nBufferLen);
				delete [] pBuffer;
				pBuffer = NULL;
			}
			////发送获取信息
			//MSG_GM_S_ClientInfo ClientInfo;
			//ClientInfo.bEnableWatch=m_pGameInfo->bEnableWatch;
			//SendGameData(&ClientInfo,sizeof(ClientInfo),MDM_GM_GAME_FRAME,ASS_GM_GAME_INFO,0);

			break;
		}
	default:
		break;
	}
	return 0;
}
//网络读取消息
bool CLoveSendClassForEXE::OnSocketReadEvent(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket)
{
	//过滤处理
	if (m_pGameInfo->pMeUserInfo->GameUserInfo.bDeskNO==255) 
		return true;
	//处理消息
	switch (pNetHead->bMainID)
	{
	case MDM_GM_GAME_FRAME:		//框架消息
		{
			OnFrameMessage(pNetHead,pNetData,uDataSize,pClientSocket);
			break;
		}
	case MDM_GR_PROP:
		{
			OnPropMessage(pNetHead,pNetData,uDataSize,pClientSocket);
			break;
		}
	}
	//wushuqun 2009.6.18
	//此处的MDM_GR_NETSIGNAL消息为IPC模拟消息
	NetMessageHead virNetMsg;
	memset(& virNetMsg,0,sizeof(NetMessageHead));
	virNetMsg.bMainID = MDM_GR_NETSIGNAL;
	virNetMsg.bAssistantID =  MDM_GR_NETSIGNAL;
	virNetMsg.bHandleCode = MDM_GR_NETSIGNAL;
	int nBufferLen1 = sizeof(NetMessageHead);
	BYTE *pBuffer1 = new BYTE[nBufferLen1];
	memcpy( pBuffer1, &virNetMsg, sizeof(NetMessageHead) );
	SendIPCMessage( pBuffer1, nBufferLen1 );

	////////////////////////////////////////

	// 来自网络的消息，里面都是实实在在的内容，不需要分析，\
	直接把pNetHead与pNetData合并后通过IPC转发即可
	int nBufferLen = sizeof(NetMessageHead) + uDataSize;
	BYTE *pBuffer = new BYTE[nBufferLen];

	memcpy( pBuffer, pNetHead, sizeof(NetMessageHead) );
	memcpy( pBuffer + sizeof(NetMessageHead), pNetData, uDataSize );
	SendIPCMessage( pBuffer, nBufferLen );

	return true;
}
//框架消息处理函数，需要改变m_pGameInfo的状态
bool CLoveSendClassForEXE::OnFrameMessage(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket)
{
	ASSERT(pNetHead->bMainID==MDM_GM_GAME_FRAME);
	switch (pNetHead->bAssistantID)
	{
	case ASS_GM_GAME_INFO:		//游戏信息
		{
			//效验数据
			MSG_GM_S_GameInfo * pGameInfo=(MSG_GM_S_GameInfo *)pNetData;

			//处理数据
			m_bWatchOther=(pGameInfo->bWatchOther==TRUE);
			m_bWaitTime=pGameInfo->bWaitTime;
			m_bWatchMode=(m_pGameInfo->pMeUserInfo->GameUserInfo.bUserState==USER_WATCH_GAME);
			SetStationParameter(pGameInfo->bGameStation);
			return true;
		}
	}
	return false;
}
//道具消息
bool CLoveSendClassForEXE::OnPropMessage(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket)
{
	switch(pNetHead->bAssistantID)
	{
	case ASS_PROP_USEPROP:
		return OnUsePropResult(pNetHead,pNetData,uDataSize,pClientSocket);
	default:
		break;
	}
	return true;
}
//处理道具消息
bool CLoveSendClassForEXE::OnUsePropResult(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket)
{
	if(pNetHead->bHandleCode!=DTK_GR_PROP_USE_SUCCEED)
		return true;
	if(sizeof(_TAG_USINGPROP)!= uDataSize)
		return false;
	_TAG_USINGPROP * usingProp = (_TAG_USINGPROP *)pNetData;
	UserItemStruct *pUserInfo =m_pGameInfo->pIFindUser->FindOnLineUser(usingProp->dwTargetUserID);
	UserItemStruct *pUserInfoUsed =m_pGameInfo->pIFindUser->FindOnLineUser(usingProp->dwUserID);
	if(pUserInfo == NULL)
		return true;

	int attribAction=usingProp->nPropActionAttrib;
	int attribValue=usingProp->nPropValueAttrib;
	//双倍积分
	if(attribAction & 0x4)
	{
		pUserInfo->GameUserInfo.iDoublePointTime=usingProp->iDoubleTime;
	}
	if(attribAction & 0x8)
	{
		pUserInfo->GameUserInfo.iProtectTime=usingProp->iProtectTime;
	}

	if(attribAction & 0x10)
	{
		int fasciChange=(attribValue & 0xFF0000)>>16;
		if(attribAction & 0x20)
		{
			fasciChange=-fasciChange;
		}
		//pUserInfo->GameUserInfo.dwFascination+=fasciChange;
	}
	if(attribAction & 0x2)
	{
		if(usingProp->dwTargetUserID==m_pGameInfo->pMeUserInfo->GameUserInfo.dwUserID)
		{
			if(m_pGameInfo->pMeUserInfo->GameUserInfo.dwPoint<0)
				m_pGameInfo->pMeUserInfo->GameUserInfo.dwPoint=0;
		}
		if (pUserInfoUsed->GameUserInfo.dwPoint<0)
		{
			pUserInfoUsed->GameUserInfo.dwPoint = 0;
		}
	}
	FlushMessage();
	SendPlayerData(pUserInfo);
	// 如果是自己
	if ((pUserInfoUsed->GameUserInfo.dwUserID==m_pGameInfo->pMeUserInfo->GameUserInfo.dwUserID)
		||(pUserInfo->GameUserInfo.dwUserID==m_pGameInfo->pMeUserInfo->GameUserInfo.dwUserID))
	{
		SendGlbData();
	}
	this->GetParent()->BringWindowToTop();
	return true;
}

//发送游戏房间消息
void CLoveSendClassForEXE::ShowMsgInGame(TCHAR * szMsg, BYTE bNewOrSys, UINT uSize, TCHAR * szFontName,TCHAR *Title)
{
	MsgInGameStruct msg;
	// 信息本身不能为空
	if (NULL == szMsg)
	{
		return;
	}
	_tcscpy(msg.szMsg, szMsg);
	msg.bIsFontValid = false;
	msg.bIsTitleValid = false;
	if (NULL != szFontName)
	{
		msg.bIsFontValid = true;
		_tcscpy(msg.szFontName, szFontName);
	}
	if (NULL != Title)
	{
		msg.bIsTitleValid = true;
		_tcscpy(msg.szTitle, Title);
	}
	msg.bNewOrSys	= bNewOrSys;
	msg.uSize		= uSize;
	FlushMessage();
	CIPCSendCopyData SendCopyData(m_hWndChannel,m_hWnd);
	SendCopyData.SendData(IPC_MAIN_CONCTROL, IPC_SUB_SHOW_MSG_IN_GAME, (void*)&msg, sizeof(msg));

}

//发送嘟嘟消息
void CLoveSendClassForEXE::ShowMsgDudu(TCHAR * szSendName,
									   TCHAR * szTargetName,
									   TCHAR * szCharString,
									   COLORREF crTextColor,
									   bool bShowTime,
									   UINT uSize,
									   TCHAR * szFontName,
									   int iDuduType)
{
	MsgDuduStruct msgDudu;
	msgDudu.bIsFontValid = false;
	_tcscpy(msgDudu.szSendName, szSendName);
	_tcscpy(msgDudu.szTargetName, szTargetName);
	_tcscpy(msgDudu.szCharString, szCharString);
	msgDudu.crTextColor = crTextColor;
	msgDudu.bShowTime = bShowTime;
	msgDudu.uSize = uSize;
	msgDudu.iDuduType = iDuduType;
	if (NULL != szFontName)
	{
		msgDudu.bIsFontValid = true;
		_tcscpy(msgDudu.szFontName, szFontName);
	}
	FlushMessage();
	CIPCSendCopyData SendCopyData(m_hWndChannel,m_hWnd);
	SendCopyData.SendData(IPC_MAIN_CONCTROL, IPC_SUB_SHOW_DUDU, (void*)&msgDudu, sizeof(msgDudu));
}

//发送聊天消息
void CLoveSendClassForEXE::SendShortMessage(MSG_GR_RS_NormalTalk *pShortMessage)
{
}


//----------------------------------------------------------------------------//


//============================================================================\\
//IPC支持↓
//========
// 销毁IPC
bool CLoveSendClassForEXE::DestroyIPC()
{
	UnmapViewOfFile(m_pShareMemory);
	m_pShareMemory = NULL;
	CloseHandle(m_hShareMemory);
	m_hShareMemory = NULL;
	KillTimer(TIMER_HEART_BEAT);
	return true;
}
//初始化IPC
bool CLoveSendClassForEXE::InitIPC()
{
	//建立共享内存(这里可以根据情况，添加游戏的ID,服务器IP,端口等做为文件名识别)
	//sprintf(m_szShareName,"0x%s%ld%ld",IPCFILENAME,time(NULL),rand()%100);
	time_t this_time;
	time(&this_time);
	sprintf(m_szShareName,"0x%s%ld%ld", IPCFILENAME, this_time, rand()%100);

	//	strcpy(m_szShareName,"123456");

	m_hShareMemory=(HANDLE)CreateFileMapping((HANDLE)0xFFFFFFFFFFFF,NULL,PAGE_READWRITE,0,sizeof(tagShareMemory),m_szShareName);

	if (m_hShareMemory==NULL) 
	{
		AfxMessageBox("创建内存文件失败!");

		if(GetLastError()==ERROR_ALREADY_EXISTS)
			AfxMessageBox("内存文件已经存在!");

		return false;
	}

	m_pShareMemory=(tagShareMemory *)MapViewOfFile(m_hShareMemory,FILE_MAP_ALL_ACCESS,0,0,0);

	//设置共享内存
	memset(m_pShareMemory,0,sizeof(tagShareMemory));
	m_pShareMemory->wDataSize=sizeof(tagShareMemory);
	m_pShareMemory->hWndGamePlaza	=	m_hWnd;
	m_pShareMemory->hWndGameServer	=	m_hWnd;


	if (m_IPCRecvCopyData.SetChannelMessageSink(this)==false) 
		return false;

	TRACE("创建共享内存 [%s]  完成。",m_szShareName );

	return true;
}

// IPC回调接口
bool __cdecl CLoveSendClassForEXE::OnChannelMessage(const IPC_Head * pHead, const void * pIPCBuffer, WORD wDataSize, HWND hWndSend)
{
	m_dwIPCHeartBeatTick = GetTickCount();
	switch (pHead->wMainCmdID)
	{
	case IPC_MAIN_IPC_KERNEL:		//内核消息
		{
			return OnIPCKernel(pHead,pIPCBuffer,wDataSize,hWndSend);
		}
	case IPC_MAIN_SOCKET:			//网络事件
		{
			return OnIPCSocket(pHead,pIPCBuffer,wDataSize,hWndSend);
		}
	case IPC_MAIN_USER:
		{
			return OnIPCUserInfo(pHead,pIPCBuffer,wDataSize,hWndSend);
		}
	case IPC_MAIN_CONFIG:
		{
			DebugPrintf("IPC_MAIN_CONFIG时FlushMessage()");
			// 启动前就有的消息，此时发光
			FlushMessage();
			// 把Glb()里的部分数据，如用户道具信息及网址等信息发送出去
			// 收到EXE程序传来的回复，才通知游戏程序已经启动
			if (IPC_SUB_SERVER_INFO == pHead->wSubCmdID)
			{
				// 进入了游戏
				AfxGetApp()->m_pMainWnd->PostMessage(WM_USER+100,2,0);
			}
			return true;
		}
	case IPC_MAIN_PROP:
		{
			return OnIPCUseProp(pHead,pIPCBuffer,wDataSize,hWndSend);
		}
	default:
		break;
	}
	return false;
}

//IPC内核命令
bool CLoveSendClassForEXE::OnIPCKernel(const IPC_Head * pHead, const void * pIPCBuffer, WORD wDataSize, HWND hWndSend)
{
	ASSERT(pHead->wMainCmdID==IPC_MAIN_IPC_KERNEL);
	switch (pHead->wSubCmdID)
	{
	case IPC_SUB_IPC_CLIENT_CONNECT:	//连接消息
		{
			TRACE("IPC_SUB_IPC_CLIENT_CONNECT");

			//设置变量
			m_hWndChannel=hWndSend;			//获取客户端channel窗口句柄
			// 开启心跳定时器
			SetTimer(TIMER_HEART_BEAT, HEART_BEAT_SECOND*1000, NULL);

			TranslateGameInfo();
			// 把游戏窗口初始化相关的数据发送出去，玩家列表
			CIPCSendCopyData SendCopyData(m_hWndChannel,m_hWnd);
			SendCopyData.SendData(IPC_MAIN_CONFIG, IPC_SUB_SERVER_INFO, &m_GameInfo, sizeof(m_GameInfo));
			SendGlbData();
			FlushMessage();
			m_dwIPCHeartBeatTick = GetTickCount();
			return true;
		}
	case IPC_SUB_IPC_CLIENT_CLOSE:		//由EXE主动发出的关闭消息
		{
			// 客户端主动断开
			ExeClientShutDown();
			
			if (m_pGameInfo->uNameID == 10000300
				|| m_GameInfo.uNameID == 11200405
				|| m_GameInfo.uNameID == 20013400
				|| m_GameInfo.uNameID == 20014410)//090514a1JJ 强退踢出大厅
			{
				if (m_pGameInfo->pMeUserInfo!=NULL && m_pGameInfo->pMeUserInfo->GameUserInfo.bUserState == USER_PLAY_GAME)
				{
					//Mod-断线托管特殊处理-20090512
					m_pGameRoom->PostMessage(IDM_QUIT_ROOM_MSG,0,0);
				}
				/*CString str111;
				const GameInfoStruct * const pG = m_pGameInfo;
				str111.Format("UserInfo=%d UserState=%d GameStation=%d", (int)(pG->pMeUserInfo), pG->pMeUserInfo->GameUserInfo.bUserState,
					pG->bGameStation);
				AfxMessageBox(str111);*/
			}
			return true;
		}
	case IPC_SUB_CLOSE_FRAME:	// 平台重启动EXE客户端时，由EXE回应的消息
		{
			AfxGetApp()->m_pMainWnd->PostMessage(WM_USER+100,3,0);
			break;
		}
	}
	return false;
}
//网络指令
bool CLoveSendClassForEXE::OnIPCSocket(const IPC_Head * pHead, const void * pIPCBuffer, WORD wDataSize, HWND hWndSend)
{
	return true;
	struct SendGameDataStruct 
	{
		BYTE bMainID;
		BYTE bAssistantID;
		BYTE bHandleCode;
	};
	ASSERT(pHead->wMainCmdID==IPC_MAIN_SOCKET);
	switch (pHead->wSubCmdID)
	{
	case IPC_SUB_SOCKET_SEND:	//发包消息
		{
			//效验数据后转发给服务器
			//SendGameData();
			if (sizeof(SendGameDataStruct)==wDataSize)
			{
				SendGameDataStruct *pMsg = (SendGameDataStruct *)pIPCBuffer;
				m_pGameInfo->pISocketSend->SendGameData(pMsg->bMainID,
					pMsg->bAssistantID,
					pMsg->bHandleCode);
			}
			else
			{
				SendGameDataStruct *pMsg = (SendGameDataStruct *)pIPCBuffer;
				void *pData = (BYTE*)pIPCBuffer + sizeof(SendGameDataStruct);
				m_pGameInfo->pISocketSend->SendGameData(pData,
					wDataSize-sizeof(SendGameDataStruct),
					pMsg->bMainID,
					pMsg->bAssistantID,
					pMsg->bHandleCode);
			}

			return true;
		}
	case TIMER_HEART_BEAT:
		{
			// 记录时刻
			m_dwIPCHeartBeatTick = GetTickCount();
			return true;
		}
	}
	return false;
}
//网络指令
bool CLoveSendClassForEXE::OnIPCUserInfo(const IPC_Head * pHead, const void * pIPCBuffer, WORD wDataSize, HWND hWndSend)
{
	ASSERT(pHead->wMainCmdID==IPC_MAIN_USER);
	switch (pHead->wSubCmdID)
	{
	case IPC_SUB_ASK_USER_LIST:	//请求清单消息
		{
			//效验数据
			//ASSERT(wDataSize>=sizeof(CMD_Command));
			//if (wDataSize<sizeof(CMD_Command))
			//{
			//	return false;
			//}
			// 根据传来的桌号，获取同桌玩家清单
			BYTE bDeskNo = *(BYTE*)pIPCBuffer;
			CPtrArray PlayUser,WatchUser;
			m_pGameInfo->pIFindUser->FindOnLineUser(bDeskNo,PlayUser,WatchUser);
			// UserItemStruct的长度挺长，分几次传去，每次传整个结构
			FlushMessage();
			CIPCSendCopyData SendUserData(m_hWndChannel,m_hWnd);
			for (INT_PTR i=0;i<PlayUser.GetCount();i++)
			{
				UserItemStruct * pUserItem=(UserItemStruct *)PlayUser.GetAt(i);
				if (pUserItem!=NULL)
				{
					SendUserData.SendData(IPC_MAIN_USER, IPC_SUB_REPLY_USER_LIST, (void*)pUserItem, sizeof(UserItemStruct));
				}
			}
			// 不分旁观和参加游戏，由EXE端根据pUserItem->GameUserInfo.bUserState来判断是旁观还是参赛
			for (INT_PTR i=0;i<WatchUser.GetCount();i++)
			{
				UserItemStruct * pUserItem=(UserItemStruct *)WatchUser.GetAt(i);
				if (pUserItem!=NULL) 
				{
					SendUserData.SendData(IPC_MAIN_USER, IPC_SUB_REPLY_USER_LIST, (void*)pUserItem, sizeof(UserItemStruct));
				}
			}

			m_bIsGameValid = true;
			return true;
		}
		//////////////////////////////////////////////////////////////////////////
		//add by 曾宪冬
	case IPC_SUB_GAME_START:	// 从游戏程序传来的改变游戏状态的消息
		{
			BYTE bStation = *(BYTE*)pIPCBuffer;
			this->SetStationParameter(bStation);
			break;
		}
		//////////////////////////////////////////////////////////////////////////
	}
	return false;
}
//网络指令
bool CLoveSendClassForEXE::OnIPCUseProp(const IPC_Head * pHead, const void * pIPCBuffer, WORD wDataSize, HWND hWndSend)
{
	ASSERT(pHead->wMainCmdID==IPC_MAIN_PROP);
	switch (pHead->wSubCmdID)
	{
	case IPC_SUB_BROADCAST_BIG:	//广播，不分大小，大小由pIPCBuffer里判断
		{
			BYTE *pBuffer = (BYTE*)pIPCBuffer;
			_TAG_BOARDCAST *pBroad = (_TAG_BOARDCAST *)pBuffer;
			int nBroadcastType = *((int*)(pBuffer + sizeof(_TAG_BOARDCAST)));
			// 根据类型，调用相应函数
			if (nBroadcastType == BRD_MSG_BIG)
			{
				//GetMainRoom()->m_TCPSocket.SendData(pBroad,sizeof(_TAG_BOARDCAST),MDM_GP_PROP,ASS_PROP_BIG_BOARDCASE,0);
			}
			else
			{
				//SendGameData(pBroad,sizeof(_TAG_BOARDCAST),MDM_GR_PROP,ASS_PROP_SMALL_BOARDCASE,0);
			}
			return true;
		}
	case IPC_SUB_BUY_PROP:	// 买道具
		{
			if (wDataSize != sizeof(_TAG_PROP_BUY))
			{
				return false;
			}
			_TAG_PROP_BUY *pPropBuy = (_TAG_PROP_BUY *)pIPCBuffer;
			//GetMainRoom()->m_TCPSocket.SendData(pPropBuy, wDataSize, MDM_GP_PROP, ASS_PROP_BUY, 0);
			//DebugPrintf("GetMainRoom()->m_TCPSocket.SendData(pPropBuy, %d, MDM_GP_PROP, ASS_PROP_BUY, 0);", wDataSize);
			return true;
		}
	case IPC_SUB_PRESENT_PROP:	// 送道具给别人
		{
			if (wDataSize != sizeof(_TAG_PROP_GIVE))
			{
				return false;
			}
			_TAG_PROP_GIVE *pPropGive = (_TAG_PROP_GIVE *)pIPCBuffer;
			//GetMainRoom()->m_TCPSocket.SendData(pPropGive, sizeof(_TAG_PROP_GIVE), MDM_GP_PROP, ASS_PROP_GIVE, 0);
			DebugPrintf("[%s]送[%s][%d]个[%d]，值[%d]---Dev",
				pPropGive->szUser, pPropGive->szTargetUser, pPropGive->nGiveCount, pPropGive->nPropID, pPropGive->iPropValue);

			return true;
		}
	}
	return false;
}
// 发送IPC消息
bool CLoveSendClassForEXE::SendIPCMessage(void * pIPCBuffer, WORD wDataSize)
{
	//////////////////////////////////////////////////////////////////////////
	// 如果客户端还没有启动，则应当把这个消息暂存起来
	//m_bIsGameValid = true;

	if ( !m_bIsGameValid || NULL == m_hWndChannel)
	{
		DebugPrintf("游客户端还没有启动，暂存信息SendIPCMessage()");
		MessageToSendStruct msg;
		msg.ipcMain = IPC_MAIN_SOCKET;
		msg.ipcSub = IPC_SUB_SOCKET_SEND;
		msg.pBuffer = (BYTE*)pIPCBuffer;
		msg.wSize = wDataSize;
		//EnterCriticalSection(&m_csForIPCMessage);
		m_lstIPCMessage.push_back(msg);
		//LeaveCriticalSection(&m_csForIPCMessage);
		return false;
	}
	else
	{
//		DebugPrintf("游客户端已经启动，发送信息, SendIPCMessage()");
		FlushMessage();
		CIPCSendCopyData SendCopyData(m_hWndChannel,m_hWnd);
		SendCopyData.SendData(IPC_MAIN_SOCKET, IPC_SUB_SOCKET_SEND, pIPCBuffer, wDataSize);
		delete [] pIPCBuffer;
		pIPCBuffer = NULL;
		return true;
	}
}
// 
BOOL CLoveSendClassForEXE::OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct)
{
	if (m_IPCRecvCopyData.OnCopyData(pWnd->GetSafeHwnd(), pCopyDataStruct)==true) 
		return TRUE;

	return CWnd::OnCopyData(pWnd, pCopyDataStruct);
}

// IPC支持↑
//============================================================================//


//退出程序时需要通知服务器，如果已经启动了客户端程序，还需要把EXE程序关掉
void CLoveSendClassForEXE::OnCancel()
{
	//查询状态
	if ((m_bWatchMode==false)&&GetStationParameter() >= 20 && GetStationParameter() < 23)//!CanLeftDesk())
	{
		SendGameData(MDM_GM_GAME_FRAME,ASS_GM_FORCE_QUIT,0);
	}
	if (m_pGameInfo->pMeUserInfo->GameUserInfo.bDeskNO!=255) 
		SendGameData(MDM_GR_USER_ACTION,ASS_GR_USER_UP,0);

	return;
}

/// 停止EXE进程
int CLoveSendClassForEXE::StopGameClient()
{
	if (NULL ==m_hWndChannel)
		return -1;

	m_bIsGameValid = false;
	//关闭客户端程序，通过发送消息
	CIPCSendCopyData SendCopyData(m_hWndChannel,m_hWnd);
	SendCopyData.SendData(IPC_MAIN_CONCTROL,IPC_SUB_CLOSE_FRAME);

	// 清空原句柄
	m_hWndChannel = NULL;

	//	OnCancel();
	return 0;
}

// 启动EXE进程
int CLoveSendClassForEXE::StartGameClient()
{
	//判断是否已经启动
	ASSERT(m_pShareMemory!=NULL);

	if( NULL != m_hWndChannel )
	{
		return -1;
	}
	TranslateGameInfo();
	//构造命令行
	CString strCommonLine;
	strCommonLine.Format(TEXT("%d/%s /RoomToken:%s"),
		//m_pGameInfo->szProcessName,
		m_pGameInfo->uNameID,
		"GameLauncher.exe",
		m_szShareName);	// 从游戏信息里获得EXE程序的命令行

	//启动游戏客户端
	STARTUPINFO StartInfo;
	memset(&StartInfo,0,sizeof(StartInfo));
	StartInfo.cb=sizeof(StartInfo);
	StartInfo.wShowWindow=SW_SHOWMAXIMIZED;
	BOOL bSuccess=CreateProcess(NULL,strCommonLine.GetBuffer(),NULL,NULL,FALSE,CREATE_DEFAULT_ERROR_MODE,NULL,NULL,&StartInfo,&m_GameProcessInfo);
	strCommonLine.ReleaseBuffer();
	if (bSuccess==FALSE)
	{
		memset(&m_GameProcessInfo,0,sizeof(m_GameProcessInfo));
		return SM_CREATE_ERROR;
	}
	return 0;
}

void CLoveSendClassForEXE::TranslateGameInfo(void)
{
	m_GameInfo.bDeskOnwer		= m_pGameInfo->bDeskOnwer;
	m_GameInfo.bGameStation		= m_pGameInfo->bGameStation;						//游戏状态
	memcpy(&m_GameInfo.uisMeUserInfo, m_pGameInfo->pMeUserInfo, sizeof(m_GameInfo.uisMeUserInfo));	//玩家信息
	m_GameInfo.dwGamePower		= m_pGameInfo->dwGamePower;							//用户权限
	m_GameInfo.dwMasterPower	= m_pGameInfo->dwMasterPower;						//管理权限
	m_GameInfo.dwRoomRule		= m_pGameInfo->dwRoomRule;							//设置规则

	m_GameInfo.bEnableSound		= m_pGameInfo->bEnableSound;						//允许声音
	m_GameInfo.bEnableWatch		= m_pGameInfo->bEnableWatch;						//允许旁观
	m_GameInfo.bShowUserInfo	= m_pGameInfo->bShowUserInfo;						//显示进出
	m_GameInfo.bEnableBackSound = m_pGameInfo->bEnableBackSound;					//背景音乐

	m_GameInfo.uDeskPeople		= m_pGameInfo->uDeskPeople;						//游戏人数
	m_GameInfo.uRoomID			= m_pGameInfo->uRoomID;							//房间表示
	m_GameInfo.dwGameMSVer		= m_pGameInfo->dwGameMSVer;						//主版本号
	m_GameInfo.dwGameLSVer		= m_pGameInfo->dwGameLSVer;						//副版本号
	m_GameInfo.uComType			= m_pGameInfo->uComType;						//游戏类型
	m_GameInfo.uNameID			= m_pGameInfo->uNameID;							//名字表示
	m_GameInfo.uLessPoint		= m_pGameInfo->uLessPoint;						//最少经验值
	_tcscpy(m_GameInfo.szProcessName, m_pGameInfo->szProcessName);				//进程名字，如“zjh.exe”
	_tcscpy(m_GameInfo.szGameName, m_pGameInfo->szGameName);					//游戏名字，如“扎金花”

	m_GameInfo.pOrderName		= NULL;								//等级函数到EXE端变成无用，须另行产生

	//wushuqun 2009.6.3
	m_GameInfo.bIsInRecord = m_pGameInfo->bIsInRecord;
}

// 将消息队列里的内容先发出去
void CLoveSendClassForEXE::FlushMessage(void)
{
	if (!m_bIsGameValid)
	{
		DebugPrintf("客户端未准备好，返回，暂不发信息");
		return;
	}
	
	//EnterCriticalSection(&m_csForIPCMessage);
	CIPCSendCopyData SendCopyData(m_hWndChannel,m_hWnd);
	if (m_lstIPCMessage.size()==0)
	{
		return;
	}
	try
	{
		
		std::list<MessageToSendStruct>::iterator it = m_lstIPCMessage.begin();
		while (it!=m_lstIPCMessage.end())
		{
			CM_UserState_Change_ForExe *p = (CM_UserState_Change_ForExe *)(it->pBuffer + sizeof(UINT));
			UINT *pTemp = (UINT*)(it->pBuffer);
			DebugPrintf("FlushMessage, uControlCode =[%d] actCode=[%d]", *pTemp, p->bActionCode);
			SendCopyData.SendData(it->ipcMain, it->ipcSub, it->pBuffer, it->wSize);
			delete []it->pBuffer;
			it->pBuffer = NULL;
			m_lstIPCMessage.pop_front();
			it = m_lstIPCMessage.begin();
		}
	}
	catch (...)
	{
		//LeaveCriticalSection(&m_csForIPCMessage);
		exit(-1);
	}
	//LeaveCriticalSection(&m_csForIPCMessage);
}
void CLoveSendClassForEXE::SendGlbData(void)
{
	// 有下列几项内容需要添加 \\
	1、道具\
	2、主页\
	3、网址

	CIPCSendCopyData SendCopyData(m_hWndChannel,m_hWnd);
	// 取道具里的内容，是个指针数组，需要遍历内容再发送
	int nPropCount = Glb().userPropLibrary.GetCount();
	if (nPropCount>0)
	{
		// 从Glb()中获取道具指针，逐条发送
		_TAG_USERPROP *userProp=NULL;
		for (int curPropIndex=0; curPropIndex<nPropCount; ++curPropIndex)
		{
			userProp=Glb().userPropLibrary.GetAt(curPropIndex);
			SendCopyData.SendData(IPC_MAIN_PROP, IPC_SUB_INIT_PROP, userProp, sizeof(_TAG_USERPROP));
		}
	}
	SendCopyData.SendData(IPC_MAIN_PROP, IPC_SUB_INIT_PROP_FINISH, &nPropCount, sizeof(int)); // 表示发送结束
	return;
}
void CLoveSendClassForEXE::SendBringToTopData(void)
{
	// 把同桌某玩家数据再次传送过去，因为在使用道具的时候，玩家数据会有变化
	FlushMessage();
	CIPCSendCopyData SendCopyData(m_hWndChannel,m_hWnd);
	SendCopyData.SendData(IPC_MAIN_CONCTROL, IPC_SUB_JOIN_IN_GAME, 0, 0);
	return;
}
void CLoveSendClassForEXE::SendPlayerData(UserItemStruct *pUserInfo)
{
	// 把同桌某玩家数据再次传送过去，因为在使用道具的时候，玩家数据会有变化
	FlushMessage();
	CIPCSendCopyData SendCopyData(m_hWndChannel,m_hWnd);
	SendCopyData.SendData(IPC_MAIN_USER, IPC_SUB_USER_COME, pUserInfo, sizeof(UserItemStruct));
	return;
}
void CLoveSendClassForEXE::OnTimer(UINT_PTR nIDEvent)
{
	switch(nIDEvent)
	{
	case TIMER_HEART_BEAT:
		{
			if (NULL==m_hWndChannel)
			{
				KillTimer(TIMER_HEART_BEAT);
				return;
			}
			DWORD dwThisTick = GetTickCount();
			if (dwThisTick - m_dwIPCHeartBeatTick > (HEART_BEAT_DELAY+HEART_BEAT_SECOND)*1000) // 加上延时
			{
				//表示EXE端已经停止了，可以做各种处理，比如模拟断线，模拟EXE客户端主动关闭
				//这里是模拟EXE服务器端发送关闭消息
				SendGameData(MDM_GM_GAME_FRAME,ASS_GM_FORCE_QUIT,0);
				ExeClientShutDown();
				this->SetStationParameter(0);

				//Mod-断线托管特殊处理-20090512
				if (m_GameInfo.uNameID == 10000300
					|| m_GameInfo.uNameID == 11200405
					|| m_GameInfo.uNameID == 20013400
					|| m_GameInfo.uNameID == 20014410)
				{
					if (m_pGameInfo->pMeUserInfo!=NULL && m_pGameInfo->pMeUserInfo->GameUserInfo.bUserState == USER_PLAY_GAME)
					{
						//Mod-断线托管特殊处理-20090512
						m_pGameRoom->PostMessage(IDM_QUIT_ROOM_MSG,0,0);
					}
				}
			}
			else
			{
				FlushMessage();
				CIPCSendCopyData SendCopyData(m_hWndChannel,m_hWnd);
				SendCopyData.SendData(IPC_MAIN_SOCKET, IPC_SUB_IPC_HEART_BEAT, NULL, 0);
			}
			break;
		}
	default:
		break;
	}
}

void CLoveSendClassForEXE::ExeClientShutDown()
{
	// 清空原句柄
	if (NULL != m_hWndChannel)
	{
		m_hWndChannel = NULL;

		//EnterCriticalSection(&m_csForIPCMessage);
		// 清空消息队列里的内容
		std::list<MessageToSendStruct>::iterator itBegin = m_lstIPCMessage.begin();
		std::list<MessageToSendStruct>::iterator itEnd = m_lstIPCMessage.end();
		std::list<MessageToSendStruct>::iterator it = m_lstIPCMessage.begin();
		for (; it != m_lstIPCMessage.end(); ++it)
		{
			DebugPrintf("删除队列里的消息");
			delete []it->pBuffer;
			it->pBuffer = NULL;
		}
		m_lstIPCMessage.clear();
		//LeaveCriticalSection(&m_csForIPCMessage);
	}

	// 向服务器发送玩家离桌消息
	OnCancel();
	AfxGetApp()->m_pMainWnd->PostMessage(WM_USER+100,3,0);
	KillTimer(TIMER_HEART_BEAT);
}
