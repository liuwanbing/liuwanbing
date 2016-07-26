#include "StdAfx.h"
#include "AddRecordModule.h"

typedef IRecorderForPlatform * (CreateRecordPlatForm)();

CAddRecordModule::CAddRecordModule(void)
{
	m_hRecordInstance = NULL;
	m_pRecordMoudlePlat = NULL;
	m_bIsRecordAvailable = false;
}

CAddRecordModule::~CAddRecordModule(void)
{
	ReleaseRecordMoule();  //清空某些东西
}

//录像插件用到的函数
bool CAddRecordModule::CreatRecorMoudle(const TCHAR* pAppPath,bool bIsRecordAvailable)
{
	if (pAppPath == NULL)
	{
		m_bIsRecordAvailable = false;
		return false;
	}
	m_bIsRecordAvailable = bIsRecordAvailable;
	if (m_bIsRecordAvailable == false)
	{
		return false;
	}
	
	TCHAR filename[600];	
	wsprintf(filename,TEXT("%sRecordModule.dll"),pAppPath);	
	CFileFind ff;
	bool bexist=ff.FindFile(filename);
	if (!bexist)
	{
		m_bIsRecordAvailable = false;
		return false;
	}
	if (m_pRecordMoudlePlat == NULL)
	{
		m_hRecordInstance = LoadLibrary(filename);
		if (m_hRecordInstance == NULL)
		{	
			//AFCCloseFrame();
			return false;
		}

		CreateRecordPlatForm* pCreatPlayer = (CreateRecordPlatForm*)(GetProcAddress(m_hRecordInstance,GETFORPLATFORM));
		if (pCreatPlayer == NULL)
		{

			//throw TEXT("初始化對象失敗!");

			return false;
		}
		m_pRecordMoudlePlat = pCreatPlayer();
		if (m_pRecordMoudlePlat == NULL)
		{		
			//throw TEXT("初始化對象失敗!");
			return false;
		}
	}
	

	m_pRecordMoudlePlat->InitAll();    //初始化所有参数

	return true;
}

void CAddRecordModule::InitRecordMoudle(GameInfoStruct* sourcegameinfo,TCHAR* szClienDllName,BYTE bMyDeskStation)      //初始化录像插件
{
	if (m_bIsRecordAvailable == false)
	{
		return;
	}
	if (m_pRecordMoudlePlat == NULL)
	{
		return;
	}
	m_pRecordMoudlePlat->InitAll();
	CPRStructExchange exchange;
	RecordGameInfoStruct gaminfo;
	exchange.ChangeGameRoomPlatToReClient(&gaminfo,sourcegameinfo,szClienDllName,bMyDeskStation);
	m_pRecordMoudlePlat->AddRoomGameInfo(&gaminfo);
}
void CAddRecordModule::ReleaseRecordMoule()    //释放录像插件
{
	if (m_pRecordMoudlePlat != NULL)
	{
		m_pRecordMoudlePlat->Release();
	}
	if (m_hRecordInstance != NULL)
	{
		FreeLibrary(m_hRecordInstance);	
	}
}
bool CAddRecordModule::SaveRecordFile(const TCHAR* pAppPath,const TCHAR* pMyName,UINT uNameId)
{
	//目录保存结构为  执行目录/log/log_username/time/
	if (m_bIsRecordAvailable == false)
	{
		return false;
	}
	if (pAppPath == NULL || pMyName == NULL)
	{
		return false;
	}
	if (m_pRecordMoudlePlat == NULL)
	{
		return false;
	}

	TCHAR  path[1000];
	wsprintf(path,TEXT("%s"),pAppPath) ;// + TEXT("VideoCfg.ini");

	TCHAR FileName[1000];

	//创建log 目录
	wsprintf(FileName, TEXT("log"),path);
	::CreateDirectory(FileName,NULL);

	//创建用户名log
	wsprintf(path,TEXT("%s\\log_%s"),FileName,pMyName);
	::CreateDirectory(path,NULL);

    

	CTime t=CTime::GetCurrentTime();
	CString NowStr=t.Format("%Y%m%d%H%M%S");//格式化当前时间
	CString direct = t.Format("%Y-%m-%d");

	TCHAR FileName2[1000];
	wsprintf(FileName2,TEXT("%s\\%s"),path,direct);
	::CreateDirectory(FileName2,NULL);   //继续创建目录

	TCHAR file[200];
	
	wsprintf(file,TEXT("%d-%s.zxh"),uNameId,NowStr.GetBuffer());
	

	wsprintf(path,TEXT("%s\\%s"),FileName2,file);	//写入文件

	m_pRecordMoudlePlat->SaveFileWithPassZip(path);
	//写完文件之后清空
	m_pRecordMoudlePlat->ClearAllMsg();
	m_pRecordMoudlePlat->ClearAllUserInfo();

}

void CAddRecordModule::SetUserInfo2Record(const UserItemStruct* pUserItem,const UserItemStruct* pMeUserInfo)
{
	if (m_bIsRecordAvailable == false)
	{
		return;
	}

	if (pUserItem == NULL || pMeUserInfo == NULL)
	{
		return;
	}
	RecordUserItemStruct RUserItem;
	CPRStructExchange exchange;
	exchange.ChaneUserItemPlatToReClient(&RUserItem,pUserItem);
	
	if ((pUserItem->GameUserInfo.dwUserID == pMeUserInfo->GameUserInfo.dwUserID) || 
		(pUserItem->GameUserInfo.bDeskStation == pMeUserInfo->GameUserInfo.bDeskStation && pUserItem->GameUserInfo.bDeskNO == pMeUserInfo->GameUserInfo.bDeskNO) )
	{
		//exchange.ChaneUserItemPlatToReClient(&RUserItem,pMeUserInfo);
		if (pUserItem->GameUserInfo.bDeskStation < MAX_PLAYER_COUNT)
		{
			m_pRecordMoudlePlat->AddMeDeskStation(pUserItem->GameUserInfo.bDeskStation);  //加入自己的座位号
		}
		
	}	
	
	m_pRecordMoudlePlat->AddUserInfo(&RUserItem);

}

void CAddRecordModule::AddMsgToRecord(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize)
{
	if (m_bIsRecordAvailable == false)
	{
		return;
	}
	if (m_pRecordMoudlePlat == NULL)
	{
		return;
	}
	//录入游戏消息和场景消息yjj 090223
	if (pNetHead->bMainID == MDM_GM_GAME_NOTIFY || 
		(pNetHead->bMainID==MDM_GM_GAME_FRAME && (pNetHead->bAssistantID == ASS_GM_GAME_STATION || pNetHead->bAssistantID == ASS_GM_GAME_INFO)))
	{	
		//往录像插件加入消息
		m_pRecordMoudlePlat->AddMsg2Record(pNetHead,sizeof(NetMessageHead),pNetData,uDataSize);		
	}
}
bool CAddRecordModule::GetUserGameState(const GameInfoStruct* pGameInfo ,NetMessageHead * pNetHead)
{
	if (pGameInfo == NULL || pGameInfo->pMeUserInfo == NULL || pNetHead == NULL)
	{
		return false;
	}
	
	if (pNetHead->bMainID!=MDM_GM_GAME_FRAME || pNetHead->bAssistantID != ASS_GM_GAME_STATION)
	{
		return false;
	}

	if (pGameInfo->bGameStation < 20)
	{
		return false;
	}
	if (pGameInfo->bGameStation == 23) 
	{
		return false;
	}
	return true;

}
