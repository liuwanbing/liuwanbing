
#include "StdAfx.h"
#include "resource.h"
#include "GameFrameDlg.h"
#include <SYS/Stat.h>

BEGIN_MESSAGE_MAP(CLoveSendClass, CDialog)
	ON_WM_TIMER()
END_MESSAGE_MAP()


//013构造函数
CLoveSendClass::CLoveSendClass(UINT uTemplate):CDialog(uTemplate)
{
	m_pGameRoom = NULL;

	memset(m_pUserInfo,0,sizeof(m_pUserInfo));
	m_nPowerOfGold=0;
	return;
}

//析构函数
CLoveSendClass::~CLoveSendClass()
{

}


//008倥件绑定函数
void CLoveSendClass::DoDataExchange(CDataExchange * pDX)
{
	CDialog::DoDataExchange(pDX);

}

//007初始化函数
BOOL CLoveSendClass::OnInitDialog()
{
	__super::OnInitDialog();

	
	AfxMessageBox("11111111");

	return false;
}
//建立窗口
bool CLoveSendClass::AFCCreateFrame()
{
	//创建窗口

	Create(IDD_GAME_FRAME,m_pGameRoom);
	//获取当前桌上信息
	ResetGameStation();
	return true;
}

//关闭窗口
bool CLoveSendClass::AFCCloseFrame()
{
	//设置参数
	SetStationParameter(0);

	if (GetSafeHwnd()!=NULL) 
	{
		ResetGameFrame();
		this->DestroyWindow();
	}
	return true;
}
//初始化
bool CLoveSendClass::InitFrame(GameInfoStruct * pGameInfo, CWnd * pGameRoom)
{
	m_pGameInfo=pGameInfo;
	m_pGameRoom = pGameRoom;
	CINIFile forder( CINIFile::GetAppPath() + "\\Order.ini");
	m_nPowerOfGold = forder.GetKeyVal("VirtualMoney", "PowerOfGold", 0);
	return true;
}


//控制消息 
UINT CLoveSendClass::OnControlMessage(UINT uControlCode, void * pControlData, UINT uDataSize)
{
	//处理数据
	switch (uControlCode)
	{
	case CM_USER_STATE:		//用户状态改变
		{
			//效验数据
			if (uDataSize!=sizeof(CM_UserState_Change)) return 1;
			CM_UserState_Change * pStateChange=(CM_UserState_Change *)pControlData;

			//处理数据
			switch (pStateChange->bActionCode)
			{
			case ACT_USER_UP:		//用户起来
				//case ACT_WATCH_UP:		//旁观起来
				{
					return 0;
				}
			case ACT_USER_SIT:		//用户坐下
				//	case ACT_WATCH_SIT:		//旁观坐下
				if (pStateChange->pUserItem!=m_pGameInfo->pMeUserInfo)   //别人进来
				{
					//GameUserCome(pStateChange->bDeskStation,pStateChange->pUserItem,pStateChange->bActionCode==ACT_WATCH_SIT);
					m_pUserInfo[pStateChange->bDeskStation] = pStateChange->pUserItem;

				}
				else   //自己进来
				{
					BYTE bMeDeskStation=m_pGameInfo->pMeUserInfo->GameUserInfo.bDeskNO;
					if (bMeDeskStation==255)
						return 0;

					//获取用户资料
					CPtrArray PlayUser,WatchUser;
					m_pGameInfo->pIFindUser->FindOnLineUser(bMeDeskStation,PlayUser,WatchUser);

					for (INT_PTR i=0;i<PlayUser.GetCount();i++)
					{
						UserItemStruct * pUserItem=(UserItemStruct *)PlayUser.GetAt(i);
						if (pUserItem!=NULL) 
							m_pUserInfo[pUserItem->GameUserInfo.bDeskStation] = pUserItem;
						//GameUserCome(pUserItem->GameUserInfo.bDeskStation,pUserItem,false);*/
					}

					//发送获取信息
					MSG_GM_S_ClientInfo ClientInfo;
					::memset(&ClientInfo,0,sizeof(MSG_GM_S_ClientInfo));
					CString ad=CINIFile::GetAppPath ();
					CINIFile f( ad + "AIMD5Pass.ini");
					//CINIFile f(_T("AIMD5Pass.ini"));
					CString key = TEXT("AiConfig");
					ClientInfo.bEnableWatch =f.GetKeyVal(key,"EnableWatch",0);
					SendGameData(&ClientInfo,sizeof(ClientInfo),MDM_GM_GAME_FRAME,ASS_GM_GAME_INFO,0);
					return 0;
				}
			case ACT_USER_CUT:		//用户断线
				{
					return 0;
				}
			case ACT_USER_AGREE:	//用户同意
				{
					return 0;
				}
			case ACT_GAME_BEGIN:	//游戏开始
			case ACT_GAME_END:		//游戏结束
				{
					return 0;
				}
			case ACT_USER_POINT:	//用户经验值
				{
					return 0;
				}
			}
		}
	}

	return 0;
}


//网络读取消息
bool CLoveSendClass::OnSocketReadEvent(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket)
{
	//过滤处理
	if (m_pGameInfo->pMeUserInfo->GameUserInfo.bDeskNO==255) 
		return true;
	//处理消息
	switch (pNetHead->bMainID)
	{
	case MDM_GM_GAME_FRAME:		//框架消息
		{
			return OnFrameMessage(pNetHead,pNetData,uDataSize,pClientSocket);
		}
	case MDM_GM_MESSAGE:		//信息消息
		{
			break;//return OnSystemMessage(pNetHead,pNetData,uDataSize,pClientSocket);
		}
	case MDM_GM_GAME_NOTIFY:	//游戏消息
		{
			return HandleGameMessage(pNetHead,pNetData,uDataSize,pClientSocket);
		}
		
	//case MDM_GR_POINT:
	//	{
	//		break;;//return HandlePointMessage(pNetHead,pNetData,uDataSize,pClientSocket);
	//	}
	}
	return true;
}


//框架消息处理函数（不需要重载）
bool CLoveSendClass::OnFrameMessage(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket)
{
	ASSERT(pNetHead->bMainID==MDM_GM_GAME_FRAME);
	switch (pNetHead->bAssistantID)
	{
	case ASS_GM_GAME_INFO:		//游戏信息
		{
			//效验数据
			MSG_GM_S_GameInfo * pGameInfo=(MSG_GM_S_GameInfo *)pNetData;

			SetStationParameter(pGameInfo->bGameStation);
			return true;
		}
	case ASS_GM_GAME_STATION:	//游戏状态
		{	
			return SetGameStation(pNetData,uDataSize);//调用DLL中的函数，，麻将中uDataSize为51
		}
	case ASS_GM_NORMAL_TALK:	//普通聊天
		{
			return true;
		}
	case ASS_GM_WATCH_SET:		//旁观设置
		{
			return true;
		}
	case ASS_GM_SET_VIDEOADDR:
		{
			return true;
		}
	}
	return false;
}

//游戏消息处理函数
bool CLoveSendClass::HandleGameMessage(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket)
{
	if (pNetHead->bMainID == MDM_GM_GAME_NOTIFY)
	{
		switch (pNetHead->bAssistantID)
		{	
		case 2:				//道俱
			{
				return true;
			}
		}
		
	}
	return true;
}


//重新设置界面
void CLoveSendClass::ResetGameFrame()
{
	m_pGameInfo->bGameStation=0;
	memset(m_pUserInfo,0,sizeof(m_pUserInfo));
	return;
}

//桌号换算到视图位置
BYTE CLoveSendClass::ViewStation(BYTE bDeskStation)
{
	return bDeskStation;
}

//014发送函数
int CLoveSendClass::SendGameData(void * pData, UINT uBufLen, BYTE bMainID, BYTE bAssistantID, BYTE bHandleCode)
{
	if (m_pGameInfo->pMeUserInfo->GameUserInfo.bDeskNO==255) 
		return 0;
	return m_pGameInfo->pISocketSend->SendGameData(pData,uBufLen,bMainID,bAssistantID,bHandleCode);
}

//发送函数
int CLoveSendClass::SendGameData(BYTE bMainID, BYTE bAssistantID, BYTE bHandleCode)
{
	if (m_pGameInfo->pMeUserInfo->GameUserInfo.bDeskNO==255)
		return 0;
	return m_pGameInfo->pISocketSend->SendGameData(bMainID,bAssistantID,bHandleCode);
}

//计时器
bool CLoveSendClass::SetGameTimer(BYTE bDeskStation,UINT uTimeCount,UINT uTimeID)
{
	if(m_pGameRoom != NULL)
		m_pGameRoom->SendMessage(IDM_SET_GAME_TIME,(WPARAM)uTimeID,(LPARAM)uTimeCount*1000);
//	OnGameTimer(bDeskStation,uTimeID,uTimeCount);
	return true;
}

//杀计时器
bool  CLoveSendClass::KillGameTimer(UINT uTimeID)
{
	if(m_pGameRoom != NULL)
		m_pGameRoom->SendMessage(IDM_KILL_GAME_TIME,(WPARAM)uTimeID,(LPARAM)uTimeID);
	return true;
}

void CLoveSendClass::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	__super::OnTimer(nIDEvent);
}
