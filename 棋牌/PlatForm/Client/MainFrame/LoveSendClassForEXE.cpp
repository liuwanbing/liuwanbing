// LoveSendClassForExe.cpp : 实现文件


#include "StdAfx.h"
#include "LoveSendClassForEXE.h"
#include "MainRoomEx.h"

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
	fprintf(fp,"[%s]--", CTime::GetCurrentTime().Format("%Y-%m-%d %H:%M:%S"));
	vfprintf(fp, p, arg );
	fprintf(fp,"\n");
	fclose(fp);
}

IMPLEMENT_DYNAMIC(CLoveSendClassForEXE, CDialog)

CLoveSendClassForEXE::CLoveSendClassForEXE(void)
{
	m_bIsGameStarting = false;
	m_hWndChannel = NULL;
	m_bWatchMode=true;
	m_bWatchOther=false;

    // PengJiLin, 2010-6-29, 混战场时间是否到
    m_bTimeOut = false;

	m_iSocketID = INVALID_SOCKET;
	m_pService = NULL;
	m_pSocket = NULL;
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
	if (m_pGameInfo->pMeUserInfo->GameUserInfo.bDeskNO==255) 
		return 0;
	return m_pGameInfo->pISocketSend->SendGameData(pData,uBufLen,bMainID,bAssistantID,bHandleCode);
}
//发送函数
int CLoveSendClassForEXE::SendGameData(BYTE bMainID, BYTE bAssistantID, BYTE bHandleCode)
{
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
	m_pGameRoom=pGameRoom;
	m_pGameRoom->SendMessage(IDM_GET_ROOMNAME,0,(LPARAM)&(m_szGameRoomName[0]));
	return true;
}

//建立窗口
bool CLoveSendClassForEXE::AFCCreateFrame()
{
	if (m_lstIPCMessage.size() > 0)
	{
		m_lstIPCMessage.clear();
	}

	if (GetSafeHwnd()==NULL) 
	{
		AfxSetResourceHandle(GetModuleHandle(FACE_DLL_NAME));
		Create(IDD_NULL, m_pGameRoom);
		AfxSetResourceHandle(GetModuleHandle(NULL));
	}
	else
	{
		StartGameClient();
	}

	return false;
}
//关闭窗口
bool CLoveSendClassForEXE::AFCCloseFrame()
{
	StopGameClient();

	if ((m_pGameInfo != NULL) && (m_pGameInfo->dwRoomRule & GRR_GAME_U3D))
	{
		if (m_pGameRoom != NULL)
			((CGameRoomEx*)m_pGameRoom)->SetBkMusic(true);
	}
	//DestroyIPC();
	if (GetSafeHwnd()!=NULL) 
	{
		ShowWindow (SW_HIDE);
		//DestroyWindow();
	}
	//CWnd * winApp=AfxGetApp()->m_pMainWnd;
	//if(winApp && winApp->m_hWnd)
	//	winApp->PostMessage(WM_USER+100,3,0);
	m_bIsGameStarting = false;
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
	case CM_U3D_GAMEBASEINFO:
		{
			OutputDebugString("serv CLoveSendClassForEx::OnControlMessage CM_U3D_GAMEBASEINFO");
			SendSocketMsg(IPC_MAIN_GAMEBASEINFO, IPC_SUB_GAMEBASEINFO, pControlData, uDataSize);
			break;
		}
	case CM_U3D_APPLICATION_QUIT:
		{
			OutputDebugString("quit: CLoveSendClassForEx:OnControlMessage CM_U3D_APPLICATION_QUIT");
			SendSocketMsg(IPC_MAIN_GAMEBASEINFO, 31, 0, 0);
			break;
		}
	case CM_USER_SEND_TIMES_MONEY: // 用户获赠金币
		{
			//send to game talking
			// 数据结构里也含指针，不能直接打包发送到EXE进程
			int nBufferLen = sizeof(UINT) + sizeof(CM_UserState_Send_Times_Money_For_Exe);
			BYTE *pBuffer = new BYTE[nBufferLen];

			memcpy(pBuffer, &uControlCode, sizeof(UINT) );
			CM_UserState_Send_Times_Money_For_Exe *pDes = (CM_UserState_Send_Times_Money_For_Exe *)(pBuffer + sizeof(UINT));
			CM_UserState_Send_Times_Money *pSrc = (CM_UserState_Send_Times_Money*)pControlData;
			pDes->dwUserID = pSrc->dwUserID;

            pDes->dwGetMoney = pSrc->dwGetMoney;
            pDes->dwMoneyOnTimes = pSrc->dwMoneyOnTimes;
            pDes->dwMoneyOnCounts = pSrc->dwMoneyOnCounts;
            pDes->dwTimesNeed = pSrc->dwTimesNeed;
            pDes->dwCountsNeed = pSrc->dwCountsNeed;

			FlushMessage();
			CIPCSendCopyData SendCopyData(m_hWndChannel,m_hWnd);
			SendCopyData.SendData(IPC_MAIN_USER, IPC_SUB_USER_SCORE, (void*)pBuffer, (WORD)nBufferLen);

			SendSocketMsg(IPC_MAIN_USER, IPC_SUB_USER_SCORE, (void*)pBuffer, (WORD)nBufferLen);

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
				// 再通知EXE游戏程序
				memcpy( &p->uisUserItem, pStateChange->pUserItem, sizeof(UserItemStruct) );
				
				if (m_pGameInfo->dwRoomRule & GRR_GAME_U3D) {
					
					if (m_pGameInfo->uNameID == 30311800) {				//发送小猪快跑消息
						
						SendXzkpMsg(&p->uisUserItem, IPC_MAIN_USER, IPC_SUB_USER_STATUS);
					} else /*if (m_pGameInfo->uNameID == 20100400)*/ {		//发送大众麻将消息
						
						if (pStateChange->bActionCode == ACT_USER_SIT || 
							pStateChange->bActionCode == ACT_WATCH_SIT) {
							
							CString str; 
							str.Format("serv:  LoveSendClassForEXE  发送坐下玩家的信息 -> %d   自己的id -> %d", p->uisUserItem.GameUserInfo.bDeskStation, m_pGameInfo->pMeUserInfo->GameUserInfo.bDeskStation);
							OutputDebugString(str);

							u3dUserBaseInfo temp;
							u3dCopyDzmjData(&temp, &p->uisUserItem, m_pGameInfo);
							//temp.iUserState = 3;
							SendSocketMsg(IPC_MAIN_USERINFO, IPC_SUB_USERSITDOWN, &temp, sizeof(u3dUserBaseInfo));
						}
						else if(pStateChange->bActionCode == ACT_USER_UP || 
							pStateChange->bActionCode == ACT_WATCH_UP)
						{
							SendSocketMsg(10, 3, &p->uisUserItem.GameUserInfo.dwUserID, sizeof(long int));
						}
					}
				}
			}
			//////////////////////////////////////////////////////////////////////////
			//add by 曾宪冬

			// 如果是自己的消息
			if (p->uisUserItem.GameUserInfo.dwUserID == m_pGameInfo->pMeUserInfo->GameUserInfo.dwUserID)
			{
				// 自己离桌的消息，不需要缓存
				// 在GameRoomEx.cpp中收到用户消息时，已经将bActionCode作了重新赋值，因此这里改为ACT_XXX的宏
				if ((p->bActionCode == ACT_USER_UP)
					||(p->bActionCode == ACT_WATCH_UP)
					||(p->bActionCode == ACT_USER_CUT))
				{
					delete [] pBuffer;
					pBuffer = NULL;
					break;
				}

			}

			//////////////////////////////////////////////////////////////////////////
			/// 如果客户端还没有启动完成，则应当把这个消息暂存起来
			if (NULL == m_hWndChannel)
			{
				/// 但如果游戏没有正在启动，则不必存储
				if (m_bIsGameStarting)
				{
					MessageToSendStruct msg;
					msg.ipcMain = IPC_MAIN_USER;
					msg.ipcSub = IPC_SUB_USER_STATUS;
					msg.pBuffer = pBuffer;
					msg.wSize = nBufferLen;
					m_lstIPCMessage.push_back(msg);					
				}
			}
			else /// 游戏客户端已经启动完成，则直接发送消息
			{
				//DebugPrintf("游客户端已经启动，发送信息, p->bActionCode = [%d]", p->bActionCode);
				FlushMessage();
				CIPCSendCopyData SendCopyData(m_hWndChannel,m_hWnd);
				SendCopyData.SendData(IPC_MAIN_USER, IPC_SUB_USER_STATUS, (void*)pBuffer, (WORD)nBufferLen);
				
				SendSocketMsg(IPC_MAIN_USER, IPC_SUB_USER_STATUS, (void*)pBuffer, (WORD)nBufferLen);

				delete [] pBuffer;
				pBuffer = NULL;
			}
			break;
		}
	case CM_USER_PROP:
		{
			FlushMessage();
			CIPCSendCopyData SendCopyData(m_hWndChannel,m_hWnd);
			SendCopyData.SendData(IPC_USER_PROP_RESULT, 0, (void*)pControlData, (WORD)uDataSize);

			SendSocketMsg(IPC_USER_PROP_RESULT, 0, (void*)pControlData, (WORD)uDataSize);

		}
		break;
	case CM_USER_TROTTING:
		{
			//JianGuankun 2012.1.12 游戏跑马灯中消息
			FlushMessage();
			CIPCSendCopyData SendCopyData(m_hWndChannel,m_hWnd);
			SendCopyData.SendData(IPC_TROTTING, 0, (void*)pControlData, (WORD)uDataSize);
			
			SendSocketMsg(IPC_TROTTING, 0, (void*)pControlData, (WORD)uDataSize);
		}
		break;

	case CM_EX_SKIN:
		{
			CIPCSendCopyData SendCopyData(m_hWndChannel,m_hWnd);
			SendCopyData.SendData(IPC_EX_SKIN, 0,pControlData,uDataSize);
		}
		break;
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
	{
		return true;
	}
		
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
	case MDM_GR_MONEY:	///< 来自银行的消息，已经在GameRoomX处理过了，只须转发给游戏进程，不需要再次处理
		{
			break;
		}
    case MDM_GR_ROOM:   //混战场时间到
        {
            if(ASS_GR_BATTLEROOM_RESULT == pNetHead->bAssistantID)
            {
                m_bTimeOut = true;
                return true;    // 与其他消息不一样，直接返回
            }
        }
	default:
		break;
	}
	// 来自网络的消息，里面都是实实在在的内容，不需要分析，\
	直接把pNetHead与pNetData合并后通过IPC转发即可
	int nBufferLen = sizeof(NetMessageHead) + uDataSize;
	BYTE *pBuffer = new BYTE[nBufferLen];

	memcpy( pBuffer, pNetHead, sizeof(NetMessageHead) );
	memcpy( pBuffer + sizeof(NetMessageHead), pNetData, uDataSize );

	//通过管道向GameLauncher发送消息
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

			SendSocketMsg(MDM_GM_GAME_FRAME, ASS_GM_GAME_INFO, (BYTE *)pNetData, uDataSize);

			return true;
		}
	case ASS_GM_GAME_STATION:
		{			
			if (m_pGameInfo->pMeUserInfo->GameUserInfo.bDeskNO != 255)
			{
				SendDeskUsers(m_pGameInfo->pMeUserInfo->GameUserInfo.bDeskNO);
			}

			SendSocketMsg(MDM_GM_GAME_FRAME, ASS_GM_GAME_STATION, (BYTE *)pNetData, uDataSize);
			return true;
		}
	case ASS_GM_NORMAL_TALK:
		{				
			SendSocketMsg(MDM_GM_GAME_FRAME,ASS_GM_NORMAL_TALK, (BYTE *)pNetData, uDataSize);			
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
	SendSocketMsg(IPC_MAIN_CONCTROL, IPC_SUB_SHOW_MSG_IN_GAME, (void*)&msg, sizeof(msg));
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
	
	SendSocketMsg(IPC_MAIN_CONCTROL, IPC_SUB_SHOW_DUDU, (void*)&msgDudu, sizeof(msgDudu));
}

//发送聊天消息
void CLoveSendClassForEXE::SendShortMessage(MSG_GR_RS_NormalTalk *pShortMessage)
{
}


//获取游戏是否运行
bool CLoveSendClassForEXE::GetGameIsStarting()
{
	return m_bIsGameStarting;
}

//----------------------------------------------------------------------------//


//============================================================================\\
//IPC支持↓
//========
// 销毁IPC
bool CLoveSendClassForEXE::DestroyIPC()
{
	UnmapViewOfFile(m_pShareMemory);
	CloseHandle(m_hShareMemory);
	m_hShareMemory = NULL;
	KillTimer(TIMER_HEART_BEAT);

	if (m_pSocket != NULL) delete m_pSocket;
	m_pSocket = NULL;

	if (m_pService != NULL) delete m_pService;
	m_pService = NULL;

	return true;
}
//初始化IPC
bool CLoveSendClassForEXE::InitIPC()
{
	//建立共享内存(这里可以根据情况，添加游戏的ID,服务器IP,端口等做为文件名识别)
	//sprintf(m_szShareName,"0x%s%ld%ld",IPCFILENAME,time(NULL),rand()%100);
	DWORD dwTick = GetTickCount();
	
	sprintf(m_szShareName,"0x%s%d%d%d", IPCFILENAME, dwTick, rand()%100, GetMeUserInfo()->dwUserID);

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

	m_pService = new CServerSocketSelect();
	m_pService->SetChannel(this);
	m_pSocket = new CTCPServerSocket(m_pService);

	return true;
}

// IPC回调接口
bool __cdecl CLoveSendClassForEXE::OnChannelMessage(const IPC_Head * pHead, const void * pIPCBuffer, WORD wDataSize, HWND hWndSend)
{
	m_dwIPCHeartBeatTick = 0;//GetTickCount();
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
	case MDM_GM_GAME_NOTIFY:
		{
			if (m_pGameInfo->dwRoomRule & GRR_GAME_U3D) 
			{
				return OnU3DGameSocket(pHead, pIPCBuffer, wDataSize, hWndSend);
			}
		}
	case MDM_GM_GAME_FRAME: 
		{
			if (m_pGameInfo->dwRoomRule & GRR_GAME_U3D) 
			{
				if(pHead->wSubCmdID == 1 || pHead->wSubCmdID == 4 || pHead->wSubCmdID == 2)
				{
					return OnU3DFrameSocket(pHead, pIPCBuffer, wDataSize, hWndSend);
				}
			}
		}
		
	case IPC_MAIN_USER:
		{
			return OnIPCUserInfo(pHead,pIPCBuffer,wDataSize,hWndSend);
		}
	case IPC_MAIN_CONFIG:
		{
			//DebugPrintf("IPC_MAIN_CONFIG时FlushMessage()");
			// 启动前就有的消息，此时发光
			FlushMessage();
			// 把Glb()里的部分数据，如用户道具信息及网址等信息发送出去
			// 收到EXE程序传来的回复，才通知游戏程序已经启动
			if (IPC_SUB_SERVER_INFO == pHead->wSubCmdID)
			{
				// 进入了游戏
				AfxGetApp()->m_pMainWnd->PostMessage(WM_USER+100,2,0);
				m_pGameRoom->PostMessage(WM_GAME_LAUNCHED, 0, 0);
			}

			return true;
		}
	case IPC_MAIN_PROP:
		{
			return OnIPCUseProp(pHead,pIPCBuffer,wDataSize,hWndSend);
		}
	case IPC_PROP_BUY_VIP:
		{
			return OnIPCBuyVIP(pHead,pIPCBuffer,wDataSize,hWndSend);
		}
	case IPC_FRIEND:
		{
			if (pHead->wSubCmdID == IPC_ASS_CHECKFRIEND)
			{
				int _id = *((int*)pIPCBuffer);
				HTREEITEM htiUser= GetMainRoom()->m_pGameListWnd->m_IMList.FindUserITEM(_id+1000);
				int b = GetMainRoom()->m_pGameListWnd->m_IMList.GetParentItem(htiUser) == GetMainRoom()->m_pGameListWnd->m_IMList.htiFamilier;
				CIPCSendCopyData SendCopyData(m_hWndChannel,m_hWnd);
				SendCopyData.SendData(IPC_FRIEND, IPC_ASS_CHECKFRIEND, &b, sizeof(int));
				SendSocketMsg(IPC_FRIEND, IPC_ASS_CHECKFRIEND, &b, sizeof(int));
			}
			else if (pHead->wSubCmdID == IPC_ASS_ADDFRIEND)
			{
				MSG_IM_C_ADDREQUEST* msg = (MSG_IM_C_ADDREQUEST*)pIPCBuffer;
				GetMainRoom()->SendData(msg, msg->cbSize, MDM_GP_IM,ASS_IMC_ADDREQUEST,0);
			}
			else if (pHead->wSubCmdID == IPC_ASS_GETFRIENDLIST)
			{
				CIMMain *pIMList = &(GetMainRoom()->m_pGameListWnd->m_IMList);
				HTREEITEM htiFriend = pIMList->htiFamilier;
				if (pIMList->ItemHasChildren(htiFriend))
				{
					HTREEITEM hNextItem;
					HTREEITEM hChildItem = pIMList->GetChildItem(htiFriend);

					CIPCSendCopyData SendCopyData(m_hWndChannel,m_hWnd);
					SendCopyData.SendData(IPC_FRIEND, IPC_ASS_GETFRIENDLIST);
					SendSocketMsg(IPC_FRIEND, IPC_ASS_GETFRIENDLIST, NULL, 0);

					while (hChildItem != NULL)
					{
						hNextItem = pIMList->GetNextItem(hChildItem, TVGN_NEXT);

						MSG_IM_C_GETFRIENDLIST friendList;
						friendList.dwUserID = GetMainRoom()->m_pGameListWnd->m_IMList.GetItemData(hChildItem);
						if(friendList.dwUserID<1000)
							continue;
						friendList.dwUserID-=1000;
						CString sname=GetMainRoom()->m_pGameListWnd->m_IMList.GetItemText(hChildItem);
						strcpy(friendList.sNickName, sname);
						SendCopyData.SendData(IPC_FRIEND, IPC_ASS_GETFRIENDLIST, &friendList, sizeof(MSG_IM_C_GETFRIENDLIST));

						hChildItem = hNextItem;
					}
				}
			}
			return true;
		}
	case IPC_EMENY:    // 黑名单消息 duanxiaohui 2011-11-11
		{
			if (pHead->wSubCmdID == IPC_ASS_CHECKEMENY)
			{
				int _id = *((int*)pIPCBuffer);
				HTREEITEM htiUser = GetMainRoom()->m_pGameListWnd->m_IMList.FindUserITEM(_id + 1000);
				int b = GetMainRoom()->m_pGameListWnd->m_IMList.GetParentItem(htiUser) == GetMainRoom()->m_pGameListWnd->m_IMList.htiFamilier;
				CIPCSendCopyData SendCopyData(m_hWndChannel, m_hWnd);
				SendCopyData.SendData(IPC_EMENY, IPC_ASS_CHECKEMENY, &b, sizeof(int));
				SendSocketMsg(IPC_EMENY, IPC_ASS_CHECKEMENY, &b, sizeof(int));
			}
			else if (pHead->wSubCmdID == IPC_ASS_ADDEMENY)
			{
				MSG_IM_C_SETGROUP * pCmd = (MSG_IM_C_SETGROUP *)pIPCBuffer;
				if (pCmd == NULL)
				{
					return true;
				}


				GetMainRoom()->m_pGameListWnd->m_IMList.SetUserGroup(pCmd->dwRemoteUserID, NULL, pCmd->groupID);

				HTREEITEM htiUser = GetMainRoom()->m_pGameListWnd->m_IMList.FindUserITEM(pCmd->dwRemoteUserID + 1000);
				CString strName = GetMainRoom()->m_pGameListWnd->m_IMList.GetItemText(htiUser);

				CString strMsg("");
				CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");
				strMsg.Format(fMsg.GetKeyVal("IMDlg","MoveOneToBadList","玩家[%s]已经加入到你的黑名单中"), strName);

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
			
			return true;
		}
	default:
		break;
	}

	
	return false;
}

void char2Wchar(char* pchar,wchar_t* wchar)
{
	int nwLen = ::MultiByteToWideChar(CP_ACP,0,pchar,-1,NULL,0);
	MultiByteToWideChar(CP_ACP,0,pchar,-1,wchar,nwLen);
}

//IPC内核命令
bool CLoveSendClassForEXE::OnIPCKernel(const IPC_Head * pHead, const void * pIPCBuffer, WORD wDataSize, HWND hWndSend)
{
	ASSERT(pHead->wMainCmdID==IPC_MAIN_IPC_KERNEL);
	switch (pHead->wSubCmdID)
	{
	case IPC_SUB_IPC_CLIENT_CONNECT:	//连接消息
		{
			m_iSocketID = int(hWndSend);
			TRACE("IPC_SUB_IPC_CLIENT_CONNECT");

			//设置变量
			m_hWndChannel=hWndSend;			//获取客户端channel窗口句柄

			// 开启心跳定时器
			SetTimer(TIMER_HEART_BEAT, HEART_BEAT_SECOND*1000, NULL);

			TranslateGameInfo();
			// 把游戏窗口初始化相关的数据发送出去，玩家列表
			CIPCSendCopyData SendCopyData(m_hWndChannel,m_hWnd);
			SendCopyData.SendData(IPC_MAIN_CONFIG, IPC_SUB_SERVER_INFO, &m_GameInfo, sizeof(m_GameInfo));

			if (m_GameInfo.dwRoomRule && GRR_GAME_U3D) {
				
				
				if (m_GameInfo.uNameID == 30311800) {
				
					::memset(&m_GameInfoXzkp, 0, sizeof(UserItemStructX));

					//------------------小猪快跑数据发送---------------------------
					m_GameInfoXzkp.bMatchUser					= m_GameInfo.uisMeUserInfo.bMatchUser;
					m_GameInfoXzkp.dwLogonTime					= m_GameInfo.uisMeUserInfo.dwLogonTime;
					m_GameInfoXzkp.uConnection					= m_GameInfo.uisMeUserInfo.uConnection;
					m_GameInfoXzkp.uSortIndex					= m_GameInfo.uisMeUserInfo.uSortIndex;

					m_GameInfoXzkp.GameUserInfo.bBoy			= m_GameInfo.uisMeUserInfo.GameUserInfo.bBoy;
					m_GameInfoXzkp.GameUserInfo.bDeskNO			= m_GameInfo.uisMeUserInfo.GameUserInfo.bDeskNO;
					m_GameInfoXzkp.GameUserInfo.bDeskStation	= m_GameInfo.uisMeUserInfo.GameUserInfo.bDeskStation;
					m_GameInfoXzkp.GameUserInfo.bGameMaster		= m_GameInfo.uisMeUserInfo.GameUserInfo.bGameMaster;
					m_GameInfoXzkp.GameUserInfo.bHaveVideo		= m_GameInfo.uisMeUserInfo.GameUserInfo.bHaveVideo;
					m_GameInfoXzkp.GameUserInfo.bLogoID			= m_GameInfo.uisMeUserInfo.GameUserInfo.bLogoID;
					m_GameInfoXzkp.GameUserInfo.bMember			= m_GameInfo.uisMeUserInfo.GameUserInfo.bMember;
					m_GameInfoXzkp.GameUserInfo.bTrader			= m_GameInfo.uisMeUserInfo.GameUserInfo.bTrader;
					m_GameInfoXzkp.GameUserInfo.bUserState		= m_GameInfo.uisMeUserInfo.GameUserInfo.bUserState;
					m_GameInfoXzkp.GameUserInfo.dwAccID			= m_GameInfo.uisMeUserInfo.GameUserInfo.dwAccID;
					m_GameInfoXzkp.GameUserInfo.dwExperience	= m_GameInfo.uisMeUserInfo.GameUserInfo.dwExperience;
					m_GameInfoXzkp.GameUserInfo.dwFascination	= m_GameInfo.uisMeUserInfo.GameUserInfo.dwFascination;
					m_GameInfoXzkp.GameUserInfo.dwPoint			= m_GameInfo.uisMeUserInfo.GameUserInfo.dwPoint;
					m_GameInfoXzkp.GameUserInfo.dwTax			= m_GameInfo.uisMeUserInfo.GameUserInfo.dwTax;
					m_GameInfoXzkp.GameUserInfo.dwUserID		= m_GameInfo.uisMeUserInfo.GameUserInfo.dwUserID;
					m_GameInfoXzkp.GameUserInfo.dwUserIP		= m_GameInfo.uisMeUserInfo.GameUserInfo.dwUserIP;
					m_GameInfoXzkp.GameUserInfo.i64Bank			= m_GameInfo.uisMeUserInfo.GameUserInfo.i64Bank;
					m_GameInfoXzkp.GameUserInfo.i64ContestScore	= m_GameInfo.uisMeUserInfo.GameUserInfo.i64ContestScore;
					m_GameInfoXzkp.GameUserInfo.i64Money		= m_GameInfo.uisMeUserInfo.GameUserInfo.i64Money;
					m_GameInfoXzkp.GameUserInfo.iContestCount	= m_GameInfo.uisMeUserInfo.GameUserInfo.iContestCount;
					m_GameInfoXzkp.GameUserInfo.iDoublePointTime = m_GameInfo.uisMeUserInfo.GameUserInfo.iDoublePointTime;
					m_GameInfoXzkp.GameUserInfo.iProtectTime	= m_GameInfo.uisMeUserInfo.GameUserInfo.iProtectTime;
					m_GameInfoXzkp.GameUserInfo.isVirtual		= m_GameInfo.uisMeUserInfo.GameUserInfo.isVirtual;
					m_GameInfoXzkp.GameUserInfo.iVipTime		= m_GameInfo.uisMeUserInfo.GameUserInfo.iVipTime;
					char2Wchar(m_GameInfo.uisMeUserInfo.GameUserInfo.nickName, m_GameInfoXzkp.GameUserInfo.nickName); 
					char2Wchar(m_GameInfo.uisMeUserInfo.GameUserInfo.szCity, m_GameInfoXzkp.GameUserInfo.szCity);
					char2Wchar(m_GameInfo.uisMeUserInfo.GameUserInfo.szClassName, m_GameInfoXzkp.GameUserInfo.szClassName);
					char2Wchar(m_GameInfo.uisMeUserInfo.GameUserInfo.szName, m_GameInfoXzkp.GameUserInfo.szName);
					char2Wchar(m_GameInfo.uisMeUserInfo.GameUserInfo.szOccupation, m_GameInfoXzkp.GameUserInfo.szOccupation);
					char2Wchar(m_GameInfo.uisMeUserInfo.GameUserInfo.szPhoneNum, m_GameInfoXzkp.GameUserInfo.szPhoneNum);
					char2Wchar(m_GameInfo.uisMeUserInfo.GameUserInfo.szProvince, m_GameInfoXzkp.GameUserInfo.szProvince);
					char2Wchar(m_GameInfo.uisMeUserInfo.GameUserInfo.szZone, m_GameInfoXzkp.GameUserInfo.szZone);
					char2Wchar(m_GameInfo.uisMeUserInfo.GameUserInfo.szSignDescr, m_GameInfoXzkp.GameUserInfo.szSignDescr);
					m_GameInfoXzkp.GameUserInfo.timeLeft		= m_GameInfo.uisMeUserInfo.GameUserInfo.timeLeft;
					m_GameInfoXzkp.GameUserInfo.uCutCount		= m_GameInfo.uisMeUserInfo.GameUserInfo.uCutCount;
					m_GameInfoXzkp.GameUserInfo.uDeskBasePoint	= m_GameInfo.uisMeUserInfo.GameUserInfo.uDeskBasePoint;
					m_GameInfoXzkp.GameUserInfo.uLostCount		= m_GameInfo.uisMeUserInfo.GameUserInfo.uLostCount;
					m_GameInfoXzkp.GameUserInfo.uMidCount		= m_GameInfo.uisMeUserInfo.GameUserInfo.uMidCount;
					m_GameInfoXzkp.GameUserInfo.userInfoEx1		= m_GameInfo.uisMeUserInfo.GameUserInfo.userInfoEx1;
					m_GameInfoXzkp.GameUserInfo.userInfoEx2		= m_GameInfo.uisMeUserInfo.GameUserInfo.userInfoEx2;
					m_GameInfoXzkp.GameUserInfo.userType		= m_GameInfo.uisMeUserInfo.GameUserInfo.userType;
					m_GameInfoXzkp.GameUserInfo.uWinCount		= m_GameInfo.uisMeUserInfo.GameUserInfo.uWinCount;
			
					SendSocketMsg(IPC_MAIN_CONFIG, IPC_SUB_SERVER_INFO, &m_GameInfoXzkp, sizeof(m_GameInfoXzkp));
				} else /*if (m_pGameInfo->uNameID == 20100400)*/ {
					
					OutputDebugString("flow CLoveSendClassForEXE::OnIPCKernel IPC_SUB_IPC_CLIENT_CONNECT 大众麻将");
					u3dSGameBaseInfo gamebaseinfo;
					gamebaseinfo.iDeskPeople		= m_pGameInfo->uDeskPeople;
					gamebaseinfo.iLessExperience	= m_pGameInfo->uLessPoint;
					gamebaseinfo.iPower				= m_pGameInfo->iBasePoint;
					gamebaseinfo.lRoomRule			= m_pGameInfo->dwRoomRule;
					gamebaseinfo.iGameNameID		= m_pGameInfo->uNameID;
					char2Wchar(m_pGameInfo->szGameRoomName, gamebaseinfo.szGameRoomName);
					gamebaseinfo.iContestType		= m_pGameInfo->uComType;
					gamebaseinfo.iDeskNO			= m_pGameInfo->pMeUserInfo->GameUserInfo.bDeskNO;

					SendSocketMsg(IPC_MAIN_GAMEBASEINFO, IPC_SUB_GAMEBASEINFO, &gamebaseinfo, sizeof(u3dSGameBaseInfo));
				}
			
				int nEncryptType = Glb().m_CenterServerPara.m_nEncryptType;
				SendCopyData.SendData(IPC_MAIN_CONFIG, IPC_SUB_ENCRYPT_TYPE, &nEncryptType, sizeof(int));

				CString strOut;
				strOut.Format("client-platform =>  nickName->%s", m_GameInfoXzkp.GameUserInfo.nickName);
				OutputDebugString(strOut);

			
				SendSocketMsg(IPC_MAIN_CONFIG, IPC_SUB_ENCRYPT_TYPE, &nEncryptType, sizeof(int));

				SendGlbData();
				FlushMessage();
				m_dwIPCHeartBeatTick = 0;//GetTickCount();

			} else {

				SendSocketMsg(IPC_MAIN_CONFIG, IPC_SUB_SERVER_INFO, &m_GameInfo, sizeof(m_GameInfo));

				int nEncryptType = Glb().m_CenterServerPara.m_nEncryptType;
				SendCopyData.SendData(IPC_MAIN_CONFIG, IPC_SUB_ENCRYPT_TYPE, &nEncryptType, sizeof(int));


				SendSocketMsg(IPC_MAIN_CONFIG, IPC_SUB_ENCRYPT_TYPE, &nEncryptType, sizeof(int));

				SendGlbData();
				FlushMessage();
				m_dwIPCHeartBeatTick = 0;//GetTickCount();
			}
			return true;
		}
	case IPC_SUB_IPC_CLIENT_CLOSE:		//由EXE主动发出的关闭消息
		{
			m_iSocketID = INVALID_SOCKET;
			// 客户端主动断开
			ExeClientShutDown();
			
			//if (m_pGameInfo->uNameID == 10003300
			//	|| m_GameInfo.uNameID == 11200405
			//	|| m_GameInfo.uNameID == 11200305
			//	|| m_GameInfo.uNameID == 20013400
			//	|| m_GameInfo.uNameID == 20014410
			//	|| m_pGameInfo->uNameID == 10301800	//090514a1JJ 强退踢出大厅  lym1023添加百家乐支持
			//	||m_pGameInfo->uNameID == 30501800	//添加百变骰子梭哈支持
			//	||m_pGameInfo->uNameID == 10901800	//添加百变牛牛支持
			//	||m_pGameInfo->uNameID == 11901800	//添加百变二张牌支持
			//	||m_GameInfo.uNameID == 10409400)			
			char cKey[10];
			CString sPath=CBcfFile::GetAppPath();/////本地路径
			CBcfFile fsr( sPath + "SpecialRule.bcf");
			sprintf(cKey, "%d", m_GameInfo.uNameID);
			int iResult = fsr.GetKeyVal (_T("ForceQuitAsAuto"), cKey, 0);

			if (iResult)
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

            // PengJiLin, 2010-6-29, 如果是混战场并且时间到了，关闭处理。
            if(true == m_bTimeOut)
            {
                m_pGameRoom->PostMessage(WM_GAME_TIMEOUT, 0, 0);
            }
			if (m_pGameInfo->dwRoomRule& GRR_CONTEST)
			{
				m_pGameRoom->PostMessage(IDM_QUIT_ROOM_MSG, 0, 0);
			}

			return true;
		}
	case IPC_SUB_CLOSE_FRAME:	// 平台重启动EXE客户端时，由EXE回应的消息
		{
			AfxGetApp()->m_pMainWnd->PostMessage(WM_USER+100,3,0);
			break;
		}
	case IPC_SUB_GAMEFRAME_INITED:
		{
			//游戏框架初始化完成后，对就游戏ID房间的退出按钮设为可用
			//Addad by JianGuankun 2012.2.7

			UINT uRoomID = *((UINT*)pIPCBuffer);
			
			CMainRoomEx* pMainRoom = GetMainRoom();
			RoomInfoStruct* pRoomInfo = NULL;

			if(pMainRoom) 
			{
				if(pMainRoom->GetRoomInfoStruct(&pRoomInfo))
				{
					for (BYTE i = 0; i < MAX_GAME_ROOM; i++)
					{
						if (pRoomInfo[i].bAccess && (pRoomInfo[i].pComRoomInfo.uRoomID == uRoomID))
						{
							CGameRoomEx* pGameRoom = (CGameRoomEx*)pRoomInfo[i].pGameRoomWnd;
							pGameRoom->SetEnableExitRoomBtn();
							pGameRoom->SetFastEnterBtn(FALSE);
							return true;
						}
					}
				}
			}

			break;
		}
	}
	return false;
}
//网络指令
bool CLoveSendClassForEXE::OnIPCSocket(const IPC_Head * pHead, const void * pIPCBuffer, WORD wDataSize, HWND hWndSend)
{
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
			// 当游戏同意开始游戏，如果是混战场并且时间到了，关闭处理。
			if(true == m_bTimeOut)
            {
                SendGameDataStruct *pMsg = (SendGameDataStruct *)pIPCBuffer;
                if(ASS_GM_AGREE_GAME == pMsg->bAssistantID)
                {
                    m_pGameRoom->PostMessage(WM_GAME_TIMEOUT, 0, 0);
                }
            }
			
			SendGameDataStruct *pMsg = (SendGameDataStruct *)pIPCBuffer;

			if (pMsg != NULL && pMsg->bAssistantID == ASS_GM_AGREE_GAME && pMsg->bHandleCode == 1)
			{
				UINT uRoomID = m_pGameInfo->uRoomID;
			
				CMainRoomEx* pMainRoom = GetMainRoom();
				RoomInfoStruct* pRoomInfo = NULL;

				if(pMainRoom) 
				{
					if(pMainRoom->GetRoomInfoStruct(&pRoomInfo))
					{
						for (BYTE i = 0; i < MAX_GAME_ROOM; i++)
						{
							if (pRoomInfo[i].bAccess && (pRoomInfo[i].pComRoomInfo.uRoomID == uRoomID))
							{
								CGameRoomEx* pGameRoom = (CGameRoomEx*)pRoomInfo[i].pGameRoomWnd;
								if (pGameRoom != NULL && pGameRoom->IsQueueGameRoom())
								{
									pGameRoom->m_bContinueExit = true;
									//m_pGameRoom->PostMessage(WM_CLOSEGAME, 0, 0);
									return true;
								}
							}
						}
					}
				}
			}
			//效验数据后转发给服务器
			//SendGameData();
			if (sizeof(SendGameDataStruct)==wDataSize)
			{
				m_pGameInfo->pISocketSend->SendGameData(pMsg->bMainID,
					pMsg->bAssistantID,
					pMsg->bHandleCode);
			}
			else
			{
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
			m_dwIPCHeartBeatTick = 0;
			return true;
		}
	}
	return false;
}

//U3D框架消息Socket消息发送
bool CLoveSendClassForEXE::OnU3DFrameSocket(const IPC_Head * pHead, const void * pIPCBuffer, WORD wDataSize, HWND hWndSend)
{
	struct SendGameDataStruct 
	{
		BYTE bMainID;
		BYTE bAssistantID;
		BYTE bHandleCode;
	};

	if (0==wDataSize)
	{
		m_pGameInfo->pISocketSend->SendGameData(pHead->wMainCmdID, pHead->wSubCmdID, 0);
	}
	else
	{
		SendGameDataStruct *pMsg = new SendGameDataStruct;

		CString str;
		str.Format("reci-f  SendGameDataStruct size -> %d  Wsize -> %d  pHeadsize -> %d", sizeof(SendGameDataStruct), wDataSize, sizeof(IPC_Head));
		OutputDebugString(str);

		pMsg->bMainID = pHead->wMainCmdID;
		pMsg->bAssistantID = pHead->wSubCmdID;
		pMsg->bHandleCode = 0;

		str.Format("reci-f pMsg -> bMainID -> %d  assID -> %d", pHead->wMainCmdID, pHead->wSubCmdID);
		OutputDebugString(str);

		{

		OutputDebugString("reci-f:  刷新用户列表");
		BYTE bDeskNo = m_pGameInfo->pMeUserInfo->GameUserInfo.bDeskNO;
		CPtrArray PlayUser,WatchUser;
		if (m_pGameInfo->dwRoomRule & GRR_GAME_U3D) {
			
			m_pGameInfo->pIFindUser->FindOnLineUser(bDeskNo,PlayUser,WatchUser);
		} else {
			
			m_pGameInfo->pIFindUser->FindOnLineUser(bDeskNo,PlayUser,WatchUser);
		}

		for (INT_PTR i=0;i<PlayUser.GetCount();i++)
			{
				UserItemStruct * pUserItem=(UserItemStruct *)PlayUser.GetAt(i);
				if (pUserItem!=NULL)
				{
					
					if (m_pGameInfo->dwRoomRule & GRR_GAME_U3D) {
						
						if (m_pGameInfo->uNameID == 30311800) {
							UserItemStructX tempX;
							u3dCopyXzkpData(&tempX, pUserItem);
							//SendUserData.SendData(IPC_MAIN_USER, IPC_SUB_REPLY_USER_LIST, (void*)&tempX, sizeof(UserItemStructX));
							SendSocketMsg(IPC_MAIN_USER, IPC_SUB_REPLY_USER_LIST, (void*)&tempX, sizeof(UserItemStructX));
						} else /*if (m_pGameInfo->uNameID == 20100400)*/ {
							
							OutputDebugString("reci-f:   ASS_GM_GAME_STATION...   m_pGameInfo->uNameID == 20100400");
							u3dUserBaseInfo userInfo;
							u3dCopyDzmjData(&userInfo, pUserItem, m_pGameInfo);
							CString strOut;
							strOut.Format("reci-f   ASS_GM_GAME_STATION  refreshMoney->%d   userID -> %d", userInfo.iMoney, userInfo.uiUserID);
							OutputDebugString(strOut);
							//SendUserData.SendData(IPC_MAIN_USERINFO, IPC_SUB_USERSITDOWN, (void *)&userInfo, sizeof(u3dUserBaseInfo));
							SendSocketMsg(IPC_MAIN_USERINFO, ASS_GM_GAME_STATION, (void *)&userInfo, sizeof(u3dUserBaseInfo));
						}
					}
				}
			}
		}


		void *pData = (BYTE*)pIPCBuffer;
		m_pGameInfo->pISocketSend->SendGameData(pData,wDataSize,pMsg->bMainID,pMsg->bAssistantID,pMsg->bHandleCode);

		delete pMsg;
	}

	return true;
}

//U3D游戏消息Socket消息发送
bool CLoveSendClassForEXE::OnU3DGameSocket(const IPC_Head * pHead, const void * pIPCBuffer, WORD wDataSize, HWND hWndSend)
{
	struct SendGameDataStruct 
	{
		BYTE bMainID;
		BYTE bAssistantID;
		BYTE bHandleCode;
	};
	
	if(true == m_bTimeOut) {
		if(ASS_GM_AGREE_GAME == pHead->wSubCmdID) { 
			
			m_pGameRoom->PostMessage(WM_GAME_TIMEOUT, 0, 0);
		}
	}

	if (ASS_GM_AGREE_GAME == pHead->wSubCmdID || 0 == pHead->wSubCmdID || 2 == pHead->wSubCmdID) 
	{
		OutputDebugString("reci:  刷新用户列表");
		BYTE bDeskNo = m_pGameInfo->pMeUserInfo->GameUserInfo.bDeskNO;
		CPtrArray PlayUser,WatchUser;
		if (m_pGameInfo->dwRoomRule & GRR_GAME_U3D) {
			
			m_pGameInfo->pIFindUser->FindOnLineUser(bDeskNo,PlayUser,WatchUser);
		} else {
			
			m_pGameInfo->pIFindUser->FindOnLineUser(bDeskNo,PlayUser,WatchUser);
		}

		for (INT_PTR i=0;i<PlayUser.GetCount();i++)
			{
				UserItemStruct * pUserItem=(UserItemStruct *)PlayUser.GetAt(i);
				if (pUserItem!=NULL)
				{
					
					if (m_pGameInfo->dwRoomRule & GRR_GAME_U3D) {
						
						if (m_pGameInfo->uNameID == 30311800) {
							UserItemStructX tempX;
							u3dCopyXzkpData(&tempX, pUserItem);
							//SendUserData.SendData(IPC_MAIN_USER, IPC_SUB_REPLY_USER_LIST, (void*)&tempX, sizeof(UserItemStructX));
							SendSocketMsg(IPC_MAIN_USER, IPC_SUB_REPLY_USER_LIST, (void*)&tempX, sizeof(UserItemStructX));
						} else /*if (m_pGameInfo->uNameID == 20100400)*/ {
							
							OutputDebugString("reci:   ASS_GM_AGREE_GAME...   m_pGameInfo->uNameID == 20100400");
							u3dUserBaseInfo userInfo;
							u3dCopyDzmjData(&userInfo, pUserItem, m_pGameInfo);
							CString strOut;
							strOut.Format("reci   ASS_GM_AGREE_GAME  refreshMoney->%d   userID -> %d", userInfo.iMoney, userInfo.uiUserID);
							OutputDebugString(strOut);
							//SendUserData.SendData(IPC_MAIN_USERINFO, IPC_SUB_USERSITDOWN, (void *)&userInfo, sizeof(u3dUserBaseInfo));
							SendSocketMsg(IPC_MAIN_USERINFO, 7, (void *)&userInfo, sizeof(u3dUserBaseInfo));
						}
					}
				}
			}
	}


	if (0==wDataSize)
	{
		m_pGameInfo->pISocketSend->SendGameData(pHead->wMainCmdID, pHead->wSubCmdID, 0);
	}
	else
	{
		SendGameDataStruct *pMsg = new SendGameDataStruct;

		CString str;
		str.Format("reci  SendGameDataStruct size -> %d  Wsize -> %d  pHeadsize -> %d", sizeof(SendGameDataStruct), wDataSize, sizeof(IPC_Head));
		OutputDebugString(str);

		pMsg->bMainID = pHead->wMainCmdID;
		pMsg->bAssistantID = pHead->wSubCmdID;
		pMsg->bHandleCode = 0;
		void *pData = (BYTE*)pIPCBuffer;
		m_pGameInfo->pISocketSend->SendGameData(pData,
		wDataSize,
		pMsg->bMainID,
		pMsg->bAssistantID,
		pMsg->bHandleCode);
		delete pMsg;
	}

	return true;
}

void CLoveSendClassForEXE::SendDeskUsers(int bDeskNo)
{
	CIPCSendCopyData SendUserData(m_hWndChannel,m_hWnd);
	//坐下后向游戏发送皮肤初始化信息
	SendUserData.SendData(IPC_EX_SKIN, 0, &(::GetMainRoom()->m_PlaceUserInfo), sizeof(MSG_GP_R_LogonResult));

	CString str;
	str.Format("cq::----------向GL发送第%d桌玩家", bDeskNo);
	OutputDebugString(str);
	CPtrArray PlayUser,WatchUser;
	if (m_pGameInfo->dwRoomRule & GRR_GAME_U3D) 
	{
		m_pGameInfo->pIFindUser->FindOnLineUser(bDeskNo,PlayUser,WatchUser);
	}
	else
	{
		m_pGameInfo->pIFindUser->FindOnLineUser(bDeskNo,PlayUser,WatchUser);
	}
	str.Format("cq::----------大厅中第%d桌有%d个玩家",bDeskNo, PlayUser.GetCount());
	OutputDebugString(str);
	// UserItemStruct的长度挺长，分几次传去，每次传整个结构
	FlushMessage();
	
	for (INT_PTR i=0;i<PlayUser.GetCount();i++)
	{
		UserItemStruct * pUserItem=(UserItemStruct *)PlayUser.GetAt(i);
		if (pUserItem!=NULL)
		{

			if (m_pGameInfo->dwRoomRule & GRR_GAME_U3D) 
			{

				if (m_pGameInfo->uNameID == 30311800) 
				{
					UserItemStructX tempX;
					u3dCopyXzkpData(&tempX, pUserItem);
					SendUserData.SendData(IPC_MAIN_USER, IPC_SUB_REPLY_USER_LIST, (void*)&tempX, sizeof(UserItemStructX));
					SendSocketMsg(IPC_MAIN_USER, IPC_SUB_REPLY_USER_LIST, (void*)&tempX, sizeof(UserItemStructX));
				} 
				else //if (m_pGameInfo->uNameID == 20100400) 
				{

					u3dUserBaseInfo userInfo;
					u3dCopyDzmjData(&userInfo, pUserItem, m_pGameInfo);
					CString str;
					str.Format("flow ->  收到客户端获取玩家列表的信息的消息sizeof(u3dUserBaseInfo) -> %d", sizeof(u3dUserBaseInfo));
					OutputDebugString(str);
					//if (userInfo.iDeskStation == m_pGameInfo->pMeUserInfo->GameUserInfo.bDeskStation) {
					SendUserData.SendData(IPC_MAIN_USERINFO, IPC_SUB_USERSITDOWN, (void *)&userInfo, sizeof(u3dUserBaseInfo));
					SendSocketMsg(IPC_MAIN_USERINFO, IPC_SUB_USERSITDOWN, (void *)&userInfo, sizeof(u3dUserBaseInfo));
					//} else {
					//	SendUserData.SendData(IPC_MAIN_USERINFO, IPC_SUB_USERINFOLIST, (void *)&userInfo, sizeof(u3dUserBaseInfo));
					//	SendSocketMsg(IPC_MAIN_USERINFO, IPC_SUB_USERINFOLIST, (void *)&userInfo, sizeof(u3dUserBaseInfo));
					//}
				}
			}
			else 
			{

				SendUserData.SendData(IPC_MAIN_USER, IPC_SUB_REPLY_USER_LIST, (void*)pUserItem, sizeof(UserItemStruct));
				SendSocketMsg(IPC_MAIN_USER, IPC_SUB_REPLY_USER_LIST, (void*)pUserItem, sizeof(UserItemStruct));
			}
		}
	}

	//断线重连yj
	// 			MSG_GM_S_ClientInfo ClientInfo;
	// 			ClientInfo.bEnableWatch=m_pGameInfo->bEnableWatch;
	// 			SendGameData(&ClientInfo,sizeof(ClientInfo),MDM_GM_GAME_FRAME,ASS_GM_GAME_INFO,0);

	// 不分旁观和参加游戏，由EXE端根据pUserItem->GameUserInfo.bUserState来判断是旁观还是参赛
	for (INT_PTR i=0;i<WatchUser.GetCount();i++)
	{
		UserItemStruct * pUserItem=(UserItemStruct *)WatchUser.GetAt(i);
		if (pUserItem!=NULL) 
		{
			if (m_pGameInfo->dwRoomRule & GRR_GAME_U3D) 
			{

				UserItemStructX tempX;
				u3dCopyXzkpData(&tempX, pUserItem);
				SendUserData.SendData(IPC_MAIN_USER, IPC_SUB_REPLY_USER_LIST, (void*)&tempX, sizeof(UserItemStructX));
				SendSocketMsg(IPC_MAIN_USER, IPC_SUB_REPLY_USER_LIST, (void*)&tempX, sizeof(UserItemStructX));
			} 
			else 
			{

				SendUserData.SendData(IPC_MAIN_USER, IPC_SUB_REPLY_USER_LIST, (void*)pUserItem, sizeof(UserItemStruct));
				SendSocketMsg(IPC_MAIN_USER, IPC_SUB_REPLY_USER_LIST, (void*)pUserItem, sizeof(UserItemStruct));
			}
		}
	}


	//坐下后向游戏发送皮肤初始化信息
//	SendUserData.SendData(IPC_EX_SKIN, 0, &(::GetMainRoom()->m_PlaceUserInfo), sizeof(MSG_GP_R_LogonResult));

	m_bIsGameValid = true;

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
			if (m_pGameInfo->pMeUserInfo->GameUserInfo.bDeskNO != 255)
			{
				SendDeskUsers(bDeskNo);
			}
			m_bIsGameValid = true;
			return true;
		}
		//////////////////////////////////////////////////////////////////////////
		//add by 曾宪冬
	case IPC_SUB_GAME_START:	// 从游戏程序传来的改变游戏状态的消息
		{
			BYTE bStation = *(BYTE*)pIPCBuffer;
			if (m_hWndChannel!=NULL)
			{
				SetStationParameter(bStation);
			}
			break;
		}
		//////////////////////////////////////////////////////////////////////////
    case IPC_SUB_ACTIVE_TO_ROOM:    // PengJiLin, 2011-6-24, 激活消息
        {
            // PengJiLin, 2011-6-24, 激活消息, 手工取消激活
			BYTE bActive = *(BYTE*)pIPCBuffer;
            GetMainRoom()->SetActiveToMainRoom(bActive);
			return true;
        }
		break;
	case IPC_SUB_SCREEN_SIZE_CHANGE:
		{
			GetMainRoom()->m_TopDuWnd->OnHitMin();
			return true;
		}
		break;
	}
	return false;
}

bool CLoveSendClassForEXE::OnIPCBuyVIP(const IPC_Head * pHead, const void * pIPCBuffer, WORD wDataSize, HWND hWndSend)
{
	ASSERT(pHead->wMainCmdID==IPC_PROP_BUY_VIP);
	switch (pHead->wSubCmdID)
	{
	case IPC_ASS_BUY_VIP:	//道具面板传回的买VIP
		{
			BYTE *pBuffer = (BYTE*)pIPCBuffer;
			_TAG_PROP_BUY_VIP *pMsgVIP = (_TAG_PROP_BUY_VIP *)pBuffer;
			GetMainRoom()->SendData(pMsgVIP,sizeof(_TAG_PROP_BUY_VIP),MDM_GP_PROP,ASS_PROP_BUY_VIP,0);
			return true;
		}
	case IPC_ASS_GIVE_VIP:	// 道具面板传回的赠送VIP
		{

		}
	default:
		break;
	}
	return true;
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
				GetMainRoom()->SendData(pBroad,sizeof(_TAG_BOARDCAST),MDM_GP_PROP,ASS_PROP_BIG_BOARDCASE,0);
			}
			else
			{
				SendGameData(pBroad,sizeof(_TAG_BOARDCAST),MDM_GR_PROP,ASS_PROP_SMALL_BOARDCASE,0);
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

			GetMainRoom()->SendData(pPropBuy, wDataSize, MDM_GP_PROP, ASS_PROP_BUY, 0);

			//DebugPrintf("GetMainRoom()->SendData(pPropBuy, %d, MDM_GP_PROP, ASS_PROP_BUY, 0);", wDataSize);
			return true;
		}
    case IPC_SUB_BUY_PROP_NEW:  // PengJiLin, 2010-10-13, 商店道具的即时购买功能
        {
            if (wDataSize != sizeof(_TAG_PROP_BUY))
            {
                return false;
            }

            // 0表示在房间购买，1表示在游戏端购买
            _TAG_PROP_BUY *pPropBuy = (_TAG_PROP_BUY *)pIPCBuffer;
            GetMainRoom()->SendData(pPropBuy, wDataSize, MDM_GP_PROP, ASS_PROP_BUY_NEW, 1);
            return true;
        }
	case IPC_SUB_PRESENT_PROP:	// 送道具给别人
		{
			if (wDataSize != sizeof(_TAG_PROP_GIVE))
			{
				return false;
			}
			_TAG_PROP_GIVE *pPropGive = (_TAG_PROP_GIVE *)pIPCBuffer;
			GetMainRoom()->SendData(pPropGive, sizeof(_TAG_PROP_GIVE), MDM_GP_PROP, ASS_PROP_GIVE, 0);
			//DebugPrintf("[%s]送[%s][%d]个[%d]，值[%d]---Dev",
			//	pPropGive->szUser, pPropGive->szTargetUser, pPropGive->nGiveCount, pPropGive->nPropID, pPropGive->iPropValue);

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
		//DebugPrintf("游客户端还没有启动，暂存信息SendIPCMessage()");
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

		if (m_pGameInfo->dwRoomRule & GRR_GAME_U3D) {
		
			FlushMessage();

			OutputDebugString("noteMoney :    进入这个里面 SendSocketMsg....");
			CIPCSendCopyData SendCopyData(m_hWndChannel,m_hWnd);
			MessageToSendStruct msg;
			msg.ipcMain = IPC_MAIN_SOCKET;
			msg.ipcSub = IPC_SUB_SOCKET_SEND;
			msg.pBuffer = (BYTE*)pIPCBuffer;
			msg.wSize = wDataSize;
			NetMessageHead *p = (NetMessageHead *)(pIPCBuffer);
			SendSocketMsg(p->bMainID, p->bAssistantID, msg.pBuffer + sizeof(NetMessageHead), msg.wSize - sizeof(NetMessageHead), p->bHandleCode);
			CString str;
			str.Format("noteMoney:  bMainID -> %d  bAssistantID -> %d", p->bMainID, p->bAssistantID );
			OutputDebugString(str);
		} 
		else 
		{

			//		DebugPrintf("游客户端已经启动，发送信息, SendIPCMessage()");
			FlushMessage();
			CIPCSendCopyData SendCopyData(m_hWndChannel,m_hWnd);
			SendCopyData.SendData(IPC_MAIN_SOCKET, IPC_SUB_SOCKET_SEND, pIPCBuffer, wDataSize);
			SendSocketMsg(IPC_MAIN_SOCKET, IPC_SUB_SOCKET_SEND, pIPCBuffer, wDataSize);
			delete [] pIPCBuffer;
			pIPCBuffer = NULL;
		}

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


int CLoveSendClassForEXE::SendSocketMsg(int bMainID, int bAssistantID, void * pData, int uSize  ,DWORD dwHeadCode)
{
/*	if (m_pSocket == NULL) 
		OutputDebugString("reci -> m_pSocket == NULL");
	if (!(m_pGameInfo->dwRoomRule & GRR_GAME_U3D))
		OutputDebugString("reci m_pGameInfo->dwRoomRule != GRR_GAME_U3D");
	if (m_iSocketID == INVALID_SOCKET)
		OutputDebugString("reci m_iSocketID == INVALID_SOCKET");
*/
	if (m_pSocket != NULL && (m_pGameInfo->dwRoomRule &  GRR_GAME_U3D) && m_iSocketID != INVALID_SOCKET)
	{
//		OutputDebugString("reci  send Successs......");
		return m_pSocket->SendData(m_iSocketID, pData, uSize, bMainID, bAssistantID, dwHeadCode);
	}
	return 0;
}


//退出程序时需要通知服务器，如果已经启动了客户端程序，还需要把EXE程序关掉
void CLoveSendClassForEXE::OnCancel()
{
	/// 判断是否百人类游戏
	char cKey[10];
	CString sPath=CBcfFile::GetAppPath();
	CBcfFile fsr( sPath + "SpecialRule.bcf");
	sprintf(cKey, "%d", m_GameInfo.uNameID);
	int iResult = fsr.GetKeyVal (_T("BJLType"), cKey, 0);
	if (iResult != 0)
	{
		m_pGameRoom->PostMessage(WM_COLSE_ROOM_WITH_MSG, 0, 0);
		return;
	}


	if(!(m_pGameInfo->dwRoomRule & GRR_GAME_U3D))
	{
		if (m_pGameInfo->dwRoomRule & GRR_QUEUE_GAME)
		{
			if (m_pGameInfo->pMeUserInfo->GameUserInfo.bUserState >= 20 && m_pGameInfo->pMeUserInfo->GameUserInfo.bUserState < 23)
			{
				SendGameData(MDM_GM_GAME_FRAME,ASS_GM_FORCE_QUIT,0);
				m_pGameRoom->PostMessage(WM_COLSE_ROOM_WITH_MSG, 0, 0);
				return;
			}
			else
			{
				if (m_pGameInfo) 
				{
					if (m_pGameInfo->pMeUserInfo) 
					{
						if (m_pGameInfo->pMeUserInfo->GameUserInfo.bDeskNO!=255) 
							SendGameData(MDM_GR_USER_ACTION,ASS_GR_USER_UP,0);
					}
				}
				m_pGameRoom->PostMessage(WM_QUEUEROOM_EXIT, 0, 0);
				return;
				//m_pGameRoom->m_pQueueInRoomDlg->OnQueue();
			}
		}
	}
	
	//百人类游戏退出不需要激活，修改针对百人类游戏退出大厅崩溃问题
	// 激活消息, 手工激活
	//GetMainRoom()->SetActiveToMainRoom(1);
	//查询状态
	if ((!m_bWatchMode)&&GetStationParameter() >= 20 && GetStationParameter() < 23)//!CanLeftDesk())
	{
		SendGameData(MDM_GM_GAME_FRAME,ASS_GM_FORCE_QUIT,0);
	}

	if (m_pGameInfo) {
		if (m_pGameInfo->pMeUserInfo) {
			if (m_pGameInfo->pMeUserInfo->GameUserInfo.bDeskNO!=255) 
				SendGameData(MDM_GR_USER_ACTION,ASS_GR_USER_UP,0);
		}
	}

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
	SendSocketMsg(IPC_MAIN_CONCTROL,IPC_SUB_CLOSE_FRAME, NULL, 0);
	if (m_pSocket != NULL)
		m_pSocket->CloseClientSocket();

	// 清空原句柄
	m_hWndChannel = NULL;

	//	OnCancel();
	return 0;
}

// 启动EXE进程
int CLoveSendClassForEXE::StartGameClient()
{
	/// 判断是否已经启动
	ASSERT(m_pShareMemory!=NULL);

	if( NULL != m_hWndChannel )
	{
		return -1;
	}

	/// 游戏窗口正在启动过程中
	if (m_bIsGameStarting)
	{
		return -1;
	}
	m_bIsGameStarting = true;

	TranslateGameInfo();
	/// 构造命令行
	CString strCommonLine;

	if (m_pGameInfo->dwRoomRule & GRR_GAME_U3D)
	{
		m_hWnd = NULL;
		strCommonLine.Format(TEXT("%d\\%d.exe \"%d\""),m_pGameInfo->uNameID, m_pGameInfo->uNameID, m_pSocket->GetSocketPort());
	}
	else
	{
		strCommonLine.Format(TEXT("%d/%s /RoomToken:%s"),
			//m_pGameInfo->szProcessName,
			m_pGameInfo->uNameID,
			"游戏启动器.exe",
			m_szShareName);	// 从游戏信息里获得EXE程序的命令行
	}
	
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
	OutputDebugString("cq::------创建启动器成功");
	/// 进入游戏时关闭所有聊天框（但是玩家还是可以切换回大厅再次打开，所以没有意义）
// 	for (INT_PTR i=0;i<m_ShortMessage.GetCount();i++)
// 	{
// 		CShortMessage * pShortMessageWnd=(CShortMessage *)m_ShortMessage.GetAt(i);
// 		if(pShortMessageWnd != NULL)
// 		{
// 			pShortMessageWnd->PostMessage(WM_CLOSE, 0, 0);
// 		}
// 	}
	return 0;
}

void CLoveSendClassForEXE::TranslateGameInfo(void)
{
	m_GameInfo.bDeskOnwer		= m_pGameInfo->bDeskOnwer;
	m_GameInfo.bGameStation		= m_pGameInfo->bGameStation;						//游戏状态
	if (m_pGameInfo->pMeUserInfo) {
		memcpy(&m_GameInfo.uisMeUserInfo, m_pGameInfo->pMeUserInfo, sizeof(m_GameInfo.uisMeUserInfo));						//玩家信息
	}
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
	
	m_GameInfo.bIsInRecord = m_pGameInfo->bIsInRecord;

	//比赛专用

	m_GameInfo.iLowCount = m_pGameInfo->iLowCount;
	m_GameInfo.i64Chip = m_pGameInfo->i64Chip;
	m_GameInfo.i64TimeStart = m_pGameInfo->i64TimeStart;
	m_GameInfo.i64TimeEnd =m_pGameInfo->i64TimeEnd;
	m_GameInfo.i64LowChip = m_pGameInfo->i64LowChip;
	m_GameInfo.iBasePoint = m_pGameInfo->iBasePoint;

	strcpy(m_GameInfo.szGameRoomName,m_pGameInfo->szGameRoomName);
}

// 将消息队列里的内容先发出去
void CLoveSendClassForEXE::FlushMessage(void)
{
	if (!m_bIsGameValid)
	{
		//DebugPrintf("客户端未准备好，返回，暂不发信息");
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
			if (pTemp) 
			{
				if (m_pGameInfo->dwRoomRule & GRR_GAME_U3D) 
				{
					MessageToSendStruct msg;
					msg.ipcMain = IPC_MAIN_SOCKET;
					msg.ipcSub = IPC_SUB_SOCKET_SEND;
					msg.pBuffer = (BYTE*)it->pBuffer;
					msg.wSize = it->wSize;
					NetMessageHead *p = (NetMessageHead *)(it->pBuffer);
					SendSocketMsg(p->bMainID, p->bAssistantID, msg.pBuffer + sizeof(NetMessageHead), msg.wSize - sizeof(NetMessageHead));
					delete [] it->pBuffer;
					it->pBuffer = NULL;
				}
				else 
				{

					SendCopyData.SendData(it->ipcMain, it->ipcSub, it->pBuffer, it->wSize);
					NetMessageHead* _p = (NetMessageHead*)(it->pBuffer);
					SendSocketMsg(_p->bMainID, _p->bAssistantID, it->pBuffer+sizeof(NetMessageHead), it->wSize - sizeof(NetMessageHead));
					delete [] it->pBuffer;
					it->pBuffer = NULL;
				}
			}
			m_lstIPCMessage.pop_front();
			it = m_lstIPCMessage.begin();
			int nSize = m_lstIPCMessage.size();
			if (nSize<=0)
			{
				break;
			}
		}
	}
	catch (...)
	{
		//LeaveCriticalSection(&m_csForIPCMessage);
		DebugPrintf("FlushMessage时异常", __LINE__);
		return;
		//exit(-1);
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
			if (userProp)
			{
				SendCopyData.SendData(IPC_MAIN_PROP, IPC_SUB_INIT_PROP, userProp, sizeof(_TAG_USERPROP));
				SendSocketMsg(IPC_MAIN_PROP, IPC_SUB_INIT_PROP, userProp, sizeof(_TAG_USERPROP));
			}
		}
	}
	MsgGlbData gd;
	memset( &gd, 0, sizeof(MsgGlbData));
	gd.m_nEncryType = Glb().m_CenterServerPara.m_nEncryptType;
	_tcscpy(gd.m_NewUrl, Glb().m_NewUrl.GetBuffer());
	_tcscpy(gd.m_szToken, Glb().m_TML_SN.GetBuffer());
	SendCopyData.SendData(IPC_MAIN_PROP, IPC_SUB_INIT_PROP_FINISH, &gd, sizeof(MsgGlbData)); // 表示发送结束
	return;
}
void CLoveSendClassForEXE::SendBringToTopData(void)
{
	// 把同桌某玩家数据再次传送过去，因为在使用道具的时候，玩家数据会有变化
	FlushMessage();
	CIPCSendCopyData SendCopyData(m_hWndChannel,m_hWnd);
	SendCopyData.SendData(IPC_MAIN_CONCTROL, IPC_SUB_JOIN_IN_GAME, 0, 0);
	SendSocketMsg(IPC_MAIN_CONCTROL, IPC_SUB_JOIN_IN_GAME, 0, 0);
	return;
}
void CLoveSendClassForEXE::SendPlayerData(UserItemStruct *pUserInfo)
{
	// 把同桌某玩家数据再次传送过去，因为在使用道具的时候，玩家数据会有变化
	FlushMessage();
	CIPCSendCopyData SendCopyData(m_hWndChannel,m_hWnd);
	SendCopyData.SendData(IPC_MAIN_USER, IPC_SUB_USER_COME, pUserInfo, sizeof(UserItemStruct));
	SendSocketMsg(IPC_MAIN_USER, IPC_SUB_USER_COME, pUserInfo, sizeof(UserItemStruct));
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
			++m_dwIPCHeartBeatTick; 
			DWORD dwThisTick = GetTickCount();
			
			if(m_dwIPCHeartBeatTick>HEART_BEAT_DELAY)
			{
				//表示EXE端已经停止了，可以做各种处理，比如模拟断线，模拟EXE客户端主动关闭
				//这里是模拟EXE服务器端发送关闭消息
				SendGameData(MDM_GM_GAME_FRAME,ASS_GM_FORCE_QUIT,0);
				ExeClientShutDown();
				this->SetStationParameter(0);

				//Mod-断线托管特殊处理-20090512
				//if (m_GameInfo.uNameID == 10003300
				//	|| m_GameInfo.uNameID == 11200405
				//	|| m_GameInfo.uNameID == 11200305
				//	|| m_GameInfo.uNameID == 20013400
				//	|| m_GameInfo.uNameID == 20014410
				//	|| m_pGameInfo->uNameID == 10301800	//  lym1023添加百家乐支持
				//	||m_pGameInfo->uNameID == 30501800	//添加百变骰子梭哈支持
				//	||m_pGameInfo->uNameID == 10901800	//添加百变牛牛支持
				//	||m_pGameInfo->uNameID == 11901800	//添加百变二张牌支持
				//	||m_GameInfo.uNameID == 10409400)
				char cKey[10];
				CString sPath=CBcfFile::GetAppPath();/////本地路径
				CBcfFile fsr( sPath + "SpecialRule.bcf");
				sprintf(cKey, "%d", m_GameInfo.uNameID);
				int iResult = fsr.GetKeyVal (_T("ForceQuitAsAuto"), cKey, 0);

				if (iResult)
				{
					if (m_pGameInfo->pMeUserInfo!=NULL && m_pGameInfo->pMeUserInfo->GameUserInfo.bUserState == USER_PLAY_GAME)
					{
						//Mod-断线托管特殊处理
						m_pGameRoom->PostMessage(IDM_QUIT_ROOM_MSG,0,0);
					}
				}
			}
			else
			{
				//FlushMessage();
				CIPCSendCopyData SendCopyData(m_hWndChannel,m_hWnd);
				SendCopyData.SendData(IPC_MAIN_SOCKET, IPC_SUB_IPC_HEART_BEAT, NULL, 0);
				SendSocketMsg(IPC_MAIN_SOCKET, IPC_SUB_IPC_HEART_BEAT, NULL, 0);

				FlushMessage();
			}
			break;
		}
	default:
		break;
	}
}

void CLoveSendClassForEXE::ExeClientShutDown()
{
	BZSoundPlay(this, "music/关闭声.mp3", 0);
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
			//DebugPrintf("删除队列里的消息");
			if (it->pBuffer) {
				delete []it->pBuffer;
				it->pBuffer = NULL;
			}
		}
		m_lstIPCMessage.clear();
		//LeaveCriticalSection(&m_csForIPCMessage);
	}
	m_bIsGameStarting = false;

	// 向服务器发送玩家离桌消息
	OnCancel();
	AfxGetApp()->m_pMainWnd->PostMessage(WM_USER+100,3,0);
	KillTimer(TIMER_HEART_BEAT);
}

void CLoveSendClassForEXE::SendXzkpMsg(UserItemStruct *pUserItem, UINT mainID, UINT subID)
{
	if (pUserItem!=NULL) {
		
		UserItemStructX tempX;
		::memset(&tempX, 0, sizeof(UserItemStructX));
		tempX.bMatchUser					= pUserItem->bMatchUser;
		tempX.dwLogonTime					= pUserItem->dwLogonTime;
		tempX.uConnection					= pUserItem->uConnection;
		tempX.uSortIndex					= pUserItem->uSortIndex;

		tempX.GameUserInfo.bBoy				= pUserItem->GameUserInfo.bBoy;
		tempX.GameUserInfo.bDeskNO			= pUserItem->GameUserInfo.bDeskNO;
		tempX.GameUserInfo.bDeskStation		= pUserItem->GameUserInfo.bDeskStation;
		tempX.GameUserInfo.bGameMaster		= pUserItem->GameUserInfo.bGameMaster;
		tempX.GameUserInfo.bHaveVideo		= pUserItem->GameUserInfo.bHaveVideo;
		tempX.GameUserInfo.bLogoID			= pUserItem->GameUserInfo.bLogoID;
		tempX.GameUserInfo.bMember			= pUserItem->GameUserInfo.bMember;
		tempX.GameUserInfo.bTrader			= pUserItem->GameUserInfo.bTrader;
		tempX.GameUserInfo.bUserState		= pUserItem->GameUserInfo.bUserState;
		tempX.GameUserInfo.dwAccID			= pUserItem->GameUserInfo.dwAccID;
		tempX.GameUserInfo.dwExperience		= pUserItem->GameUserInfo.dwExperience;
		tempX.GameUserInfo.dwFascination	= pUserItem->GameUserInfo.dwFascination;
		tempX.GameUserInfo.dwPoint			= pUserItem->GameUserInfo.dwPoint;
		tempX.GameUserInfo.dwTax			= pUserItem->GameUserInfo.dwTax;
		tempX.GameUserInfo.dwUserID			= pUserItem->GameUserInfo.dwUserID;
		tempX.GameUserInfo.dwUserIP			= pUserItem->GameUserInfo.dwUserIP;
		tempX.GameUserInfo.i64Bank			= pUserItem->GameUserInfo.i64Bank;
		tempX.GameUserInfo.i64ContestScore	= pUserItem->GameUserInfo.i64ContestScore;
		tempX.GameUserInfo.i64Money			= pUserItem->GameUserInfo.i64Money;
		tempX.GameUserInfo.iContestCount	= pUserItem->GameUserInfo.iContestCount;
		tempX.GameUserInfo.iDoublePointTime = pUserItem->GameUserInfo.iDoublePointTime;
		tempX.GameUserInfo.iProtectTime		= pUserItem->GameUserInfo.iProtectTime;
		tempX.GameUserInfo.isVirtual		= pUserItem->GameUserInfo.isVirtual;
		tempX.GameUserInfo.iVipTime			= pUserItem->GameUserInfo.iVipTime;
		char2Wchar(pUserItem->GameUserInfo.nickName, tempX.GameUserInfo.nickName); 
		char2Wchar(pUserItem->GameUserInfo.szCity, tempX.GameUserInfo.szCity);
		char2Wchar(pUserItem->GameUserInfo.szClassName, tempX.GameUserInfo.szClassName);
		char2Wchar(pUserItem->GameUserInfo.szName, tempX.GameUserInfo.szName);
		char2Wchar(pUserItem->GameUserInfo.szOccupation, tempX.GameUserInfo.szOccupation);
		char2Wchar(pUserItem->GameUserInfo.szPhoneNum, tempX.GameUserInfo.szPhoneNum);
		char2Wchar(pUserItem->GameUserInfo.szProvince, tempX.GameUserInfo.szProvince);
		char2Wchar(pUserItem->GameUserInfo.szZone, tempX.GameUserInfo.szZone);
		char2Wchar(pUserItem->GameUserInfo.szSignDescr, tempX.GameUserInfo.szSignDescr);
		tempX.GameUserInfo.timeLeft			= pUserItem->GameUserInfo.timeLeft;
		tempX.GameUserInfo.uCutCount		= pUserItem->GameUserInfo.uCutCount;
		tempX.GameUserInfo.uDeskBasePoint	= pUserItem->GameUserInfo.uDeskBasePoint;
		tempX.GameUserInfo.uLostCount		= pUserItem->GameUserInfo.uLostCount;
		tempX.GameUserInfo.uMidCount		= pUserItem->GameUserInfo.uMidCount;
		tempX.GameUserInfo.userInfoEx1		= pUserItem->GameUserInfo.userInfoEx1;
		tempX.GameUserInfo.userInfoEx2		= pUserItem->GameUserInfo.userInfoEx2;
		tempX.GameUserInfo.userType			= pUserItem->GameUserInfo.userType;
		tempX.GameUserInfo.uWinCount		= pUserItem->GameUserInfo.uWinCount;

		CIPCSendCopyData SendUserData(m_hWndChannel,m_hWnd);
		SendUserData.SendData(mainID, subID, (void*)&tempX, sizeof(UserItemStructX));
		SendSocketMsg(mainID, subID, (void*)&tempX, sizeof(UserItemStructX));

		CString str;
		str.Format("tempX.GameUserInfo.nickName -> %s", pUserItem->GameUserInfo.nickName);
		OutputDebugString(str);
	}
}

void CLoveSendClassForEXE::u3dCopyXzkpData(UserItemStructX *tempX, UserItemStruct *pUserItem) 
{
	if (tempX) {
		
		::memset(tempX, 0, sizeof(UserItemStructX));
		tempX->bMatchUser					= pUserItem->bMatchUser;
		tempX->dwLogonTime					= pUserItem->dwLogonTime;
		tempX->uConnection					= pUserItem->uConnection;
		tempX->uSortIndex					= pUserItem->uSortIndex;

		tempX->GameUserInfo.bBoy				= pUserItem->GameUserInfo.bBoy;
		tempX->GameUserInfo.bDeskNO			= pUserItem->GameUserInfo.bDeskNO;
		tempX->GameUserInfo.bDeskStation		= pUserItem->GameUserInfo.bDeskStation;
		tempX->GameUserInfo.bGameMaster		= pUserItem->GameUserInfo.bGameMaster;
		tempX->GameUserInfo.bHaveVideo		= pUserItem->GameUserInfo.bHaveVideo;
		tempX->GameUserInfo.bLogoID			= pUserItem->GameUserInfo.bLogoID;
		tempX->GameUserInfo.bMember			= pUserItem->GameUserInfo.bMember;
		tempX->GameUserInfo.bTrader			= pUserItem->GameUserInfo.bTrader;
		tempX->GameUserInfo.bUserState		= pUserItem->GameUserInfo.bUserState;
		tempX->GameUserInfo.dwAccID			= pUserItem->GameUserInfo.dwAccID;
		tempX->GameUserInfo.dwExperience		= pUserItem->GameUserInfo.dwExperience;
		tempX->GameUserInfo.dwFascination	= pUserItem->GameUserInfo.dwFascination;
		tempX->GameUserInfo.dwPoint			= pUserItem->GameUserInfo.dwPoint;
		tempX->GameUserInfo.dwTax			= pUserItem->GameUserInfo.dwTax;
		tempX->GameUserInfo.dwUserID			= pUserItem->GameUserInfo.dwUserID;
		tempX->GameUserInfo.dwUserIP			= pUserItem->GameUserInfo.dwUserIP;
		tempX->GameUserInfo.i64Bank			= pUserItem->GameUserInfo.i64Bank;
		tempX->GameUserInfo.i64ContestScore	= pUserItem->GameUserInfo.i64ContestScore;
		tempX->GameUserInfo.i64Money			= pUserItem->GameUserInfo.i64Money;
		tempX->GameUserInfo.iContestCount	= pUserItem->GameUserInfo.iContestCount;
		tempX->GameUserInfo.iDoublePointTime = pUserItem->GameUserInfo.iDoublePointTime;
		tempX->GameUserInfo.iProtectTime		= pUserItem->GameUserInfo.iProtectTime;
		tempX->GameUserInfo.isVirtual		= pUserItem->GameUserInfo.isVirtual;
		tempX->GameUserInfo.iVipTime			= pUserItem->GameUserInfo.iVipTime;
		char2Wchar(pUserItem->GameUserInfo.nickName, tempX->GameUserInfo.nickName); 
		char2Wchar(pUserItem->GameUserInfo.szCity, tempX->GameUserInfo.szCity);
		char2Wchar(pUserItem->GameUserInfo.szClassName, tempX->GameUserInfo.szClassName);
		char2Wchar(pUserItem->GameUserInfo.szName, tempX->GameUserInfo.szName);
		char2Wchar(pUserItem->GameUserInfo.szOccupation, tempX->GameUserInfo.szOccupation);
		char2Wchar(pUserItem->GameUserInfo.szPhoneNum, tempX->GameUserInfo.szPhoneNum);
		char2Wchar(pUserItem->GameUserInfo.szProvince, tempX->GameUserInfo.szProvince);
		char2Wchar(pUserItem->GameUserInfo.szZone, tempX->GameUserInfo.szZone);
		char2Wchar(pUserItem->GameUserInfo.szSignDescr, tempX->GameUserInfo.szSignDescr);
		tempX->GameUserInfo.timeLeft			= pUserItem->GameUserInfo.timeLeft;
		tempX->GameUserInfo.uCutCount		= pUserItem->GameUserInfo.uCutCount;
		tempX->GameUserInfo.uDeskBasePoint	= pUserItem->GameUserInfo.uDeskBasePoint;
		tempX->GameUserInfo.uLostCount		= pUserItem->GameUserInfo.uLostCount;
		tempX->GameUserInfo.uMidCount		= pUserItem->GameUserInfo.uMidCount;
		tempX->GameUserInfo.userInfoEx1		= pUserItem->GameUserInfo.userInfoEx1;
		tempX->GameUserInfo.userInfoEx2		= pUserItem->GameUserInfo.userInfoEx2;
		tempX->GameUserInfo.userType		= pUserItem->GameUserInfo.userType;
		tempX->GameUserInfo.uWinCount		= pUserItem->GameUserInfo.uWinCount;
	}
}

void CLoveSendClassForEXE::u3dCopyDzmjData(u3dUserBaseInfo *temp, UserItemStruct *pUserItem, GameInfoStruct *pGameInfo)
{
	if (temp && pGameInfo) {
		
		::memset(temp, 0, sizeof(u3dUserBaseInfo));
		temp->uiUserID					= pUserItem->GameUserInfo.dwUserID;
		temp->iImageNO					= pUserItem->GameUserInfo.bLogoID;
		//temp->uiAge
		temp->bBoy						= pUserItem->GameUserInfo.bBoy;
		//temp->StarTag					= pUserItem->GameUserInfo.
		//temp->BornTag					= pUserItem->GameUserInfo.
		//temp->BloodTag
		temp->iLevel					= pUserItem->GameUserInfo.bMember;
		temp->iDeskNO					= pUserItem->GameUserInfo.bDeskNO;
		temp->iDeskStation				= pUserItem->GameUserInfo.bDeskStation;
		temp->iRoomID					= pGameInfo->uRoomID;
		temp->iUserState				= pUserItem->GameUserInfo.bUserState;
		temp->iMoney					= (int)pUserItem->GameUserInfo.i64Money;
		temp->iBank						= (int)pUserItem->GameUserInfo.i64Bank;
		//temp->iTreasure					= (int)pUserItem->GameUserInfo.
		temp->iFlag						= pUserItem->GameUserInfo.userType;
		temp->iWinCount					= pUserItem->GameUserInfo.uWinCount;
		temp->iLostCount				= pUserItem->GameUserInfo.uLostCount;
		temp->iDrawCount				= pUserItem->GameUserInfo.uMidCount;
		temp->iPoint					= pUserItem->GameUserInfo.dwPoint;
		char2Wchar(pUserItem->GameUserInfo.nickName, temp->szNickName);
		char2Wchar(pUserItem->GameUserInfo.szZone, temp->szCountry);
		char2Wchar(pUserItem->GameUserInfo.szProvince, temp->szProvince);
		char2Wchar(pUserItem->GameUserInfo.szCity, temp->szCity);
		char2Wchar(pUserItem->GameUserInfo.szSignDescr, temp->szGameSign);
		//temp->dwBirthday				= pUserItem->GameUserInfo.
	}
}
