#include "StdAfx.h"
#include "PlayerLogic.h"
#include "Resource.h"
//#include "../../ClientModule/FaceLib2003/MjSet.h"



//typedef IRecorderForPlatform * (CreateRecordPlatForm)();
typedef IRecorderForPlayer* (CreateRecordPlayer)();
typedef IFrameInterface * (BeginInitGoGame)();
typedef GetOrderName * (SetGameInitClass)(__int64 dwPoint);

CPlayerLogic::CPlayerLogic(CWnd* pWnd)
{
	//try
	{
		m_hGameInstance = NULL;
		m_hRecordInstance = NULL;
		m_IGameFrame = NULL;
		if (pWnd == NULL)
		{
			//throw TEXT("初始化對象失敗!");
			return;
		}
		m_pParentWnd = pWnd;
		

		
	}
	memset(&m_GameInfo,0,sizeof(m_GameInfo));
	memset(m_UserInfo,0,sizeof(m_UserInfo));
	m_uRealPlayNum = 0;
	m_iCurPlayStep = 0;
	memset(&m_CurMsg,0,sizeof(m_CurMsg));
	//catch (CException* e)
	//{
	//	AfxMessageBox(TEXT("初始化對象失敗，請稍后再試"));
	//}
	
	
}

CPlayerLogic::~CPlayerLogic(void)
{
	//释放资源和对象
	if (m_pRecordPlay != NULL)
	{
		m_pRecordPlay->Release();
	}
	if (m_IGameFrame != NULL)
	{
		m_IGameFrame->DeleteFrame();
	}
	if (m_hRecordInstance != NULL)
	{
		::FreeLibrary(m_hRecordInstance);
	}


	//ico 库为什么释放不了，还有待查找原因

	//if (m_hGameInstance != NULL)
	//{
	//	::FreeLibrary(m_hGameInstance);
	//}


}
bool CPlayerLogic::OnInit()
{
	//TCHAR lpBuffer[512];
	//GetCurrentDirectory(512,lpBuffer);
	TCHAR filename[600];
	CString path = GetAppPath();
	//path += 
	wsprintf(filename,TEXT("%sRecordModule.dll"),path.GetBuffer());
	CFileFind ff;
	bool bexist=ff.FindFile(filename);

	m_hRecordInstance = LoadLibrary(filename);
	if (m_hRecordInstance == NULL)
	{
		//m_pParentWnd->DestroyWindow();
		//throw TEXT("初始化對象失敗!");
		return false;
	}

	CreateRecordPlayer* pCreatPlayer = (CreateRecordPlayer*)(GetProcAddress(m_hRecordInstance,GETFORPLAYER));
	if (pCreatPlayer == NULL)
	{

		//throw TEXT("初始化對象失敗!");
		
		return false;
	}
	m_pRecordPlay = pCreatPlayer();
	if (m_pRecordPlay == NULL)
	{		
		//throw TEXT("初始化對象失敗!");
		return false;
	}
	return true;

}

bool CPlayerLogic::OpenFile(TCHAR* pFileName)
{
	if (m_pRecordPlay == NULL)
	{
		
		return false;
	}
	

	if (!m_pRecordPlay->LoadFileWithPassZip(pFileName))
	{
		
		return false;
	}
	RecordGameInfoStruct* pReGameInfo = m_pRecordPlay->GetRoomGameInfo();
	if (pReGameInfo == NULL)
	{
		
		return false;
	}

	
	
	m_PRStructExchange.ChangeGameRoomReToPlatClient(&m_GameInfo,pReGameInfo);
	m_uRealPlayNum = m_pRecordPlay->GetGamePlayerNum();

	m_UserManage.CleanUserInfo();

	int iIndex = 0;
	int iLoopCount = 0;
	while (iIndex <  m_uRealPlayNum || iLoopCount < MAX_PLAYER_COUNT)
	{		
		RecordUserItemStruct* reUser = m_pRecordPlay->GetUserInfo(iLoopCount);
		iLoopCount ++;
		if (reUser == NULL)
		{
			continue;
		}
		if (reUser->GameUserInfo.dwUserID <= 0)
		{
			continue;
		}
		
		m_PRStructExchange.ChaneUserItemReToPlatClient(&m_UserInfo[iIndex],reUser);
		//加载自己的用户信息
		if (reUser == m_pRecordPlay->GetMeUserInfo())
		{
			m_GameInfo.pMeUserInfo = &m_UserInfo[iIndex];			
		}
		m_UserManage.AddGameUser(&m_UserInfo[iIndex].GameUserInfo,m_UserInfo[iIndex].uSortIndex,m_UserInfo[iIndex].uConnection);
		iIndex++;
	}
    
	//for (UINT i=0; i<m_uRealPlayNum; i++)
	//{
	//	RecordUserItemStruct* reUser = m_pRecordPlay->GetUserInfo(i);
	//	if (reUser == NULL)
	//	{
	//		return false;
	//	}
	//	m_PRStructExchange.ChaneUserItemReToPlatClient(&m_UserInfo[i],reUser);
	//	//加载自己的用户信息
	//	if (reUser == m_pRecordPlay->GetMeUserInfo())
	//	{
	//		m_GameInfo.pMeUserInfo = &m_UserInfo[i];			
	//	}
	//	m_UserManage.AddGameUser(&m_UserInfo[i].GameUserInfo,m_UserInfo[i].uSortIndex,m_UserInfo[i].uConnection);
	//}
	TCHAR* szProcname = m_pRecordPlay->GetClientDllName();
	if (szProcname == NULL)
    {
		
		return false;
	}

	//RecordUserItemStruct* reuseriterm = m_pRecordPlay->GetMeUserInfo();
	//UserItemStruct

	m_GameInfo.bIsInRecord = true;  

    ClearGame();


	return StartGameClient(szProcname,m_GameInfo);

    

}

void CPlayerLogic::ClearGame()
{
	m_iCurPlayStep = 0;
	if (m_IGameFrame != NULL)
	{
		m_IGameFrame->DeleteFrame();
		m_IGameFrame = NULL;
	}
	//創建游戲對象

	//ico 库为什么释放不了，还有待查找原因
/*if (m_hGameInstance != NULL)
	{
		bool bret = ::FreeLibrary(m_hGameInstance);
		m_hGameInstance = NULL;
	}*/
}
void CPlayerLogic::InitGame()
{
	m_iCurPlayStep = 0;
	memset(&m_CurMsg,0,sizeof(m_CurMsg));

}
bool CPlayerLogic::PlayOneStep()
{
	if (m_IGameFrame == NULL)
	{
		return false;
	}
	if (m_pRecordPlay == NULL)
	{
		return false;
	}
	if (GetNextTimeSlot() < 0)
	{
		return false;
	}
	if (m_CurMsg.uHeadSize != sizeof(NetMessageHead))
	{
		return false;
	}
	m_iCurPlayStep++;
	//if ()
	//{
	//}
	bool bRet = m_IGameFrame->OnSocketReadEvent((NetMessageHead*) m_CurMsg.NetMsgHead, m_CurMsg.NetMsgData, m_CurMsg.uDataSize,NULL);
	//wushuqun 2009.6.16
	//m_IGameFrame->SetRecordView();
	return bRet;
	

}


bool CPlayerLogic::PlayToSomeStep(UINT uStep)
{
	
	if (m_pRecordPlay == NULL)
	{
		return false;
	}
	
	if (uStep >= m_pRecordPlay->GetTotalMsgCount())
	{
		return true;
	}
	if (uStep + 1 == m_iCurPlayStep) //要走的是当前步，不用做任何处理
	{
		return true;
	}

	if (uStep >= m_iCurPlayStep)
	{
		for (UINT i=m_iCurPlayStep; i<=uStep; i++)
		{
			if (!PlayOneStep())
			{
				return false;
			}
		}
	}
	else
	{
		m_iCurPlayStep = 0;
		for (UINT i=0; i<= uStep; i++)
		{
			if (!PlayOneStep())
			{
				return false;
			}
		}
	}
}

bool CPlayerLogic::BackOneStep()
{
	
	if (m_iCurPlayStep <= 0)
	{
		m_iCurPlayStep = 0;
		return true;
	}
	//m_iCurPlayStep -= 2;
	int iCurStep = m_iCurPlayStep - 2;

	return PlayToSomeStep(iCurStep);

}
bool CPlayerLogic::IsInBegin()
{
	return (m_iCurPlayStep == 0);
}
bool CPlayerLogic::IsInEnd()
{
	if (m_pRecordPlay == NULL)
	{
		return true;
	}
	return (m_iCurPlayStep == m_pRecordPlay->GetTotalMsgCount());
}
//启动游戏客户端
bool CPlayerLogic::StartGameClient(const TCHAR* szProccName,GameInfoStruct& GameInfo)
{
	if (szProccName == NULL)
	{
		return false;
	}
	TCHAR filename[600];
	CString path = GetAppPath();
	//path += 
	

	wsprintf(filename,TEXT("%s%s"),path.GetBuffer(),szProccName);

    strcpy(GameInfo.szProcessName,szProccName);
	if (m_IGameFrame==NULL)
		//if (pCreateInterface == NULL) 
	{
		// 获取扩展名，判断到底是ICO还是EXE
		CString szExtName;
		TCHAR *p = m_GameInfo.szProcessName;
		szExtName.Format(_TEXT("%s"),  p + strlen(p)-3 );
		// 将扩展名转为大写，方便比较
		szExtName.MakeUpper();
		szExtName.CompareNoCase(_TEXT("ICO"));
		TCHAR sz[40];
		wsprintf(sz, TEXT("cq::NAME=%s"), szExtName);
		OutputDebugString(sz);
		AfxMessageBox("1");
		if (0==szExtName.CompareNoCase(_TEXT("ICO")))
		{
			m_hGameInstance = LoadLibrary(filename);
			if (m_hGameInstance == NULL)
			{
				return false;
			}
			BeginInitGoGame * pCreateInterface=(BeginInitGoGame *)GetProcAddress(m_hGameInstance,"BeginInitGoGame");
			if (pCreateInterface == NULL)
			{
				return false;
			}

			m_IGameFrame = pCreateInterface();
			if (m_IGameFrame == NULL)
			{
				return false;
			}
		}
		else if (0==szExtName.CompareNoCase(TEXT("EXE")))
		{
			m_IGameFrame = new CLoveSendClassForEXE();
		}
		else
		{
			m_IGameFrame = NULL;
			return false;
		}
		//m_IGameFrame->InitFrame(&m_GameInfo,this);

		//录像插件初始化房间信息 
		//m_AddRecordModule.InitRecordMoudle(&m_GameInfo,m_pRoomInfo->szProcessName,m_GameInfo.pMeUserInfo->GameUserInfo.bDeskStation);
	}


	
	GameInfo.pIFindUser = &m_UserManage; 

	////创建数据成功，

	SetGameInitClass * pGetOrderFunc=(SetGameInitClass *)GetProcAddress(m_hGameInstance,"SetGameInitClass");
	if (pGetOrderFunc!=NULL) 
		GameInfo.pOrderName=pGetOrderFunc(GameInfo.uNameID);
	GameInfo.pISocketSend = NULL;   //不提供socket接口来发送消息
	
	GameInfo.bIsInRecord = true;  
	
	m_IGameFrame->InitFrame(&GameInfo,m_pParentWnd);
	//m_IGameFrame->SetPrize(0);   暂时屏蔽	
	m_IGameFrame->AFCCreateFrame();	
	m_IGameFrame->AFCShowFrame(SW_SHOW);


	m_IGameFrame->SetRecordView();

	//模拟玩家坐座位
	for (int i=0 ;i<m_uRealPlayNum; i++)
	{
		if (m_UserInfo[i].GameUserInfo.bDeskStation != GameInfo.pMeUserInfo->GameUserInfo.bDeskStation)
		{
			continue;
		}
		CM_UserState_Change ControlMessage;
		ControlMessage.bActionCode = ACT_USER_SIT;
		ControlMessage.bDeskStation = i;
		ControlMessage.pUserItem = &m_UserInfo[i];

		if (m_IGameFrame!=NULL)
			m_IGameFrame->OnControlMessage(CM_USER_STATE,&ControlMessage,sizeof(ControlMessage));
		
		
		//if(pUserSit->dwUserID==m_pLogonInfo->dwUserID)
		//	return true;
	}
	//m_IGameFrame->AFCShowFrame(SW_SHOW);


	//CLoveSendClass	*set=new CLoveSendClass();
	//set->Create(IDD_GAME_FRAME,NULL);
	//set->CenterWindow();
	//set->ShowWindow(SW_SHOW);//SW_SHOW
	
	return true;

	



    
}

CString CPlayerLogic::GetAppPath(BOOL bValue)
{
	if(!bValue)
		{
			char lpBuffer[MAX_STRING_SIZE];
			ZeroMemory(lpBuffer,sizeof(lpBuffer));
			GetCurrentDirectory(MAX_STRING_SIZE,lpBuffer);
			CString ss=lpBuffer;
			ss += "\\";
			return ss;
		}
		else
		{
			TCHAR szModuleName[MAX_PATH];
			ZeroMemory(szModuleName,sizeof(szModuleName));
			DWORD dwLength=GetModuleFileName(AfxGetInstanceHandle(),szModuleName,sizeof(szModuleName));
			CString filepath=szModuleName;
			CString path;
			int nSlash = filepath.ReverseFind(_T('/'));
			if (nSlash == -1)
				nSlash = filepath.ReverseFind(_T('\\'));
			if (nSlash != -1 && filepath.GetLength() > 1)
			{
				path = filepath.Left(nSlash+1);
				SetCurrentDirectory(path);
				return path;
			}
			else
			{
				char lpBuffer[MAX_STRING_SIZE];
				ZeroMemory(lpBuffer,sizeof(lpBuffer));
				GetCurrentDirectory(MAX_STRING_SIZE,lpBuffer);
				CString ss=lpBuffer;
				ss += "\\";
				return ss;
			}

		}
}

int CPlayerLogic::GetNextTimeSlot()
{
	if (m_pRecordPlay == NULL)
	{
		return -1;
	}
	UINT uTotalMsg = m_pRecordPlay->GetTotalMsgCount();
	if (m_iCurPlayStep >= uTotalMsg)
	{
		return -1;
	}
	m_CurMsg.ResetMsgLen();
	m_pRecordPlay->GetMsgInfo(m_iCurPlayStep,m_CurMsg.NetMsgHead,m_CurMsg.NetMsgData,m_CurMsg.uHeadSize,m_CurMsg.uDataSize,m_CurMsg.uTimeSlot);
	if (m_CurMsg.uTimeSlot == 0)
	{
		return 10;
	}
    return m_CurMsg.uTimeSlot;
}

int CPlayerLogic::GetSliderPos()
{
	if (m_pRecordPlay == NULL)
	{
		return 0;
	}
	int iTotalMsg = m_pRecordPlay->GetTotalMsgCount();
	if (iTotalMsg == 0)
	{
		return 0;
	}
	return (m_iCurPlayStep * 100 / iTotalMsg);

}
void CPlayerLogic::PlaySliderPos(int iPos)
{
	if (m_pRecordPlay == NULL)
	{
		return;
	}
	if (iPos < 0)
	{
		return;
	}
	int iTotalMsg = m_pRecordPlay->GetTotalMsgCount();
	int iPlayStep = iTotalMsg * iPos / 100 - 1;
	if (iPlayStep < 0)
	{
		iPlayStep = 0;
	}
	PlayToSomeStep(iPlayStep);
	return;
}
