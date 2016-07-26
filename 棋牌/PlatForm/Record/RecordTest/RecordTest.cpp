// RecordTest.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "RecordTest.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 唯一的应用程序对象

CWinApp theApp;

using namespace std;


typedef IRecorderForPlatform * (CreateRecordPlatForm)();
typedef IRecorderForPlayer* (CreateRecordPlayer)();

struct NetMessageHead
{
	DWORD						uMessageSize;						//数据包大小
	DWORD						bMainID;							//处理主类型
	DWORD						bAssistantID;						//辅助处理类型 ID
	DWORD						bHandleCode;						//数据包处理代码
	DWORD						bReserve;							//保留字段
};



int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	// 初始化 MFC 并在失败时显示错误
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: 更改错误代码以符合您的需要
		_tprintf(_T("错误: MFC 初始化失败\n"));
		nRetCode = 1;
	}
	else
	{
		// TODO: 在此处为应用程序的行为编写代码。
		//测试录像插件的可用性：
		//getcurd
	//	int isize = sizeof(NetMessageHead);
		
		TCHAR lpBuffer[128];
		GetCurrentDirectory(128,lpBuffer);
		CString spath;
		spath.Format(TEXT("%s\\RecordModule.dll"),lpBuffer);
		CFileFind ff;
		bool bexist=ff.FindFile(spath);

		HINSTANCE hHandle = LoadLibrary(spath);//+_T("\\RecordModule.dll"));
		
		DWORD dw=::GetLastError();
		if (hHandle == NULL)
		{
			printf("loadlibrary failed");
			getchar();
			return 0;
		}
		
		CreateRecordPlatForm* pCreat = (CreateRecordPlatForm*)(GetProcAddress(hHandle,GETFORPLATFORM));
		if (pCreat == NULL)
		{
			printf("get GetProcAddress is null");
			getchar();
			return 0;
		}
		IRecorderForPlatform* pRecordMoudle = pCreat();
		if (pRecordMoudle == NULL)
		{
			printf("get object is null");
			getchar();
			return 0;
		}
        pRecordMoudle->InitAll();
		RecordUserItemStruct UserInfoStructItem;
		memset(&UserInfoStructItem,0,sizeof(UserInfoStructItem));
		RecordUserInfoStruct& UserInfoStruct = UserInfoStructItem.GameUserInfo;
		UserInfoStruct.bBoy = 1;
		UserInfoStruct.bDeskNO = 1;
		UserInfoStruct.bDeskStation = 0;
		UserInfoStruct.bGameMaster = 0;
		UserInfoStruct.bLogoID = 5;
		UserInfoStruct.bMember = 1;
		UserInfoStruct.bSayHello = 1;
		UserInfoStruct.bUserState = 2;
		UserInfoStruct.bUserType = 1;
		UserInfoStruct.dwBank = 100;
		UserInfoStruct.dwExperience = 50;
		UserInfoStruct.dwFascination = 120;
		UserInfoStruct.dwMobileNum = 0;
		UserInfoStruct.dwMoney = 0;
		UserInfoStruct.dwPoint = 10000;
		UserInfoStruct.dwReputation = 111;
		UserInfoStruct.dwUserID = 901;
		UserInfoStruct.dwUserIP = 11111243214;
		UserInfoStruct.iAgencyID = 1;
		UserInfoStruct.iDoublePointTime = 0;
		UserInfoStruct.iProtectTime = 0;

		for (int i=0; i<4; i++)
		{
			UserInfoStruct.bDeskStation = i;
			UserInfoStruct.dwUserID = 901 + i;
			wsprintf(UserInfoStruct.nickName,TEXT("nickname%d"),i + 1);
			wsprintf(UserInfoStruct.szName,TEXT("name%d"),i + 1);
			pRecordMoudle->AddUserInfo(&UserInfoStructItem);
		}
		pRecordMoudle->AddMeDeskStation(0);
		RecordGameInfoStruct gameinfo;
		memset(&gameinfo,0,sizeof(gameinfo));
		gameinfo.uDiMoney = 100;
		gameinfo.uTaiMoney = 20;
		gameinfo.uDeskPeople = 4;
		wsprintf(gameinfo.szClientDLLName,TEXT("jbmj.ico"));

		pRecordMoudle->AddRoomGameInfo(&gameinfo);
		UINT uHeadSize = 20;
		UINT uDatsSize = 50;

		unsigned char* bufferhead = new unsigned char[uHeadSize];
		unsigned char* bufferdata = new unsigned char[uDatsSize];

		memset(bufferdata,1,uDatsSize);
		memset(bufferhead,2,uHeadSize);

		CFile file;
		int rr = atoi("\r");
		int nn = atoi("\n");
		if (file.Open(TEXT("2F60-4430.KAD"),CFile::modeRead))
		{
			//printf("fjkd");
			int iFileLen = file.GetLength();
			TCHAR szchar[397821];
			DWORD read = file.Read(szchar,iFileLen);
			
			int len = lstrlen(szchar);
			for (int i=len; i<iFileLen;/* i++*/)
			{
				if (szchar[i]== 0)
				{
					i++;
					continue;
				}
				UINT* uDataSize = (UINT*)(szchar + i);
				UINT data = *uDataSize;
				i += sizeof(UINT);
				UINT* uHeadSize = (UINT*)(szchar + i);
				data = *uHeadSize;
				i += sizeof(UINT);
				

			}
			

	
			
			for (int i=0; i<iFileLen;i++)
			{
				if (szchar[i] == atoi("\r"))
				{
					printf("fjkd");
				}
				if (szchar[i] == atoi("\n"))
				{
					printf("fjkd");
				}
			}
			if (read < 1024)
			{
				
			}


			
		}


		//写十条消息进去
		//for (int i=0; i<3; i++)
		//{
		//	pRecordMoudle->AddMsg2Record(bufferhead,uHeadSize,bufferdata,uDatsSize);
		//}
		//pRecordMoudle->SaveFileWithNone(TEXT("111.txt"));
		//pRecordMoudle->SaveFileWithZip(TEXT("222.txt"));
		pRecordMoudle->SaveFileWithPassZip(TEXT("333.txt"));


		//解压缩过程
		CreateRecordPlayer* pCreatPlayer = (CreateRecordPlayer*)(GetProcAddress(hHandle,GETFORPLAYER));
		if (pCreatPlayer == NULL)
		{
			printf("get GetProcAddress is null");
			getchar();
			return 0;
		}
		IRecorderForPlayer* pPlayerMoudle = pCreatPlayer();
		//if (pPlayerMoudle == NULL)
		//{
		//	printf("get object is null");
		//	getchar();
		//	return 0;
		//}
		////pPlayerMoudle->LoadFileWithNone(TEXT("111.txt"));
		////pPlayerMoudle->LoadFileWithZip(TEXT("222.txt"));
		//pPlayerMoudle->LoadFileWithPassZip(TEXT("333.txt"));



		//UINT size = pRecordMoudle->GetParDataSize();
		//unsigned char* buffer = new unsigned char[size];
		//UINT uOutSize = 0;
		//pRecordMoudle->GetParsedData(buffer,size,uOutSize);

		//pPlayerMoudle->LoadStringBuffer(buffer,uOutSize);


		//int iMsgCount = pPlayerMoudle->GetTotalMsgCount();
  //      
		//unsigned char head[MAX_MSG_HEAD_LEN];
		//unsigned char data[MAX_MSG_DATA_LEN];

		//uHeadSize = MAX_MSG_HEAD_LEN;
		//UINT uDataSize = MAX_MSG_DATA_LEN;
		//UINT iTimeSlot = -1;

		//pPlayerMoudle->GetMsgInfo(1,head,data,uHeadSize,uDataSize,iTimeSlot);
		//int iPlayNum = pPlayerMoudle->GetGamePlayerNum();
		//RecordGameInfoStruct* pGameInfo = pPlayerMoudle->GetRoomGameInfo();
		//RecordUserItemStruct* pUserInfo = pPlayerMoudle->GetUserInfo(0);
		//RecordUserItemStruct* pMesInfo = pPlayerMoudle->GetMeUserInfo();


		

		
		

		



	}
	getchar();

	return nRetCode;
}
