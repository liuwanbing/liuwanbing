#include "StdAfx.h"
#include <SYS/Stat.h>
///#include "ServiceSet.h"
#include "ServiceManage.h"
#include "MainFrm.h"
#include "MD5.h"
#include "yxyDES.h"
#include "resource.h"

#include"../Common/CommonUse.h"

CString GetCPUID()

{

	CString CPUID;

	unsigned long s1,s2;

	unsigned char vendor_id[]="------------";

	char sel;

	sel='1';

	CString VernderID;

	CString MyCpuID,CPUID1,CPUID2;

	switch(sel)

	{

	case '1':

		__asm{

			xor eax,eax      //eax=0:取Vendor信息

				cpuid    //取cpu id指令，可在Ring3级使用

				mov dword ptr vendor_id,ebx

				mov dword ptr vendor_id[+4],edx

				mov dword ptr vendor_id[+8],ecx

		}

		VernderID.Format("%s-",vendor_id);

		__asm{

			mov eax,01h   //eax=1:取CPU序列号

				xor edx,edx

				cpuid

				mov s1,edx

				mov s2,eax

		}

		CPUID1.Format("%08X%08X",s1,s2);

		__asm{

			mov eax,03h

				xor ecx,ecx

				xor edx,edx

				cpuid

				mov s1,edx

				mov s2,ecx

		}

		CPUID2.Format("%08X%08X",s1,s2);

		break;

	case '2':

		{

			__asm{

				mov ecx,119h

					rdmsr

					or eax,00200000h

					wrmsr

			}

		}

		AfxMessageBox("CPU id is disabled.");

		break;

	}

	MyCpuID = CPUID1+CPUID2;

	CPUID = MyCpuID;

	return CPUID;

}

bool checkLicense()
{
	CString strcpuid = GetCPUID();

	unsigned char szMDTemp[16];
	MD5_CTX Md5;
	Md5.MD5Update((unsigned char *)strcpuid.GetBuffer(),strcpuid.GetLength());
	Md5.MD5Final(szMDTemp);

	char m_szMD5Pass[50];
	for (int i = 0; i < 16; i ++) 
		wsprintf(&m_szMD5Pass[i * 2], "%02x", szMDTemp[i] );
	CString StrMd5CpuID = m_szMD5Pass;

	DWORD cfgHandle=cfgOpenFile("BZGameLocal.BCF");
	if(cfgHandle<0x10)
		return false;
	CString license = cfgGetValue(cfgHandle,"License","License","");
	cfgClose(cfgHandle);

	yxyDES des;
	string key = strcpuid.GetBuffer();
	des.InitializeKey(key);


	des.DecryptAnyLength(license.GetBuffer());
	string strtmp = des.GetPlaintextAnyLength();
	string lsecpuid;
	string date;
	if (strtmp.length() == 40)
	{
		lsecpuid = strtmp.substr(0, 32);
		date = strtmp.substr(32, 40);
	}



	string lcs = m_szMD5Pass;

	CTime tmnow = CTime::GetCurrentTime();
	string strNow = tmnow.Format("%Y%m%d").GetBuffer();


	if (lcs == lsecpuid &&  strNow <= date)
		return true;
	else
	{
		CString s;
		s.Format("您的服务器未注册或已过期，请与服务商联系。\n\n请将以下机器码发送给服务商，获取注册码文件：\n\n%s\n\n",strcpuid.GetBuffer());
		MessageBox(NULL, s,"提示",MB_ICONERROR);

		s="机器码已复制到您的剪贴板中，直接Ctrl+V粘贴机器码！";
		MessageBox(NULL, s,"提示",MB_ICONINFORMATION);

		OpenClipboard(NULL);
		EmptyClipboard();
		HANDLE hData=GlobalAlloc(GMEM_MOVEABLE, strcpuid.GetLength()+5); 
		if (hData==NULL)  
		{ 
			CloseClipboard(); 
			return TRUE; 
		}
		LPTSTR szMemName=(LPTSTR)GlobalLock(hData); 
		lstrcpy(szMemName,strcpuid); 
		SetClipboardData(CF_TEXT,hData); 
		GlobalUnlock(hData);  
		GlobalFree(hData); 
		CloseClipboard(); 
		return false;
	}

}

//构造函数
CServiceManage::CServiceManage(void)// : m_ClientSocket(this)
{
	if (!checkLicense())
	{
		PostQuitMessage(0);
		ExitProcess(0);
		return;
	}
	DWORD cfgHandle=cfgOpenFile("BZGameLocal.BCF");
	if(cfgHandle<0x10)
		return;

	m_ServiceName=cfgGetValue(cfgHandle,"SERVERSET","ServiceName","BZGame");

	cfgClose(cfgHandle);


	//CString s=CINIFile::GetAppPath (false);
	//CINIFile f( s + "LocalServer.ini");
	//
	//m_ServiceName=f.GetKeyVal ("www.BZW.cn","m_ServiceName","");////客户端当前版本系列号


	m_szError[0]=0;
	m_pIMessage=NULL;
}

/********************************************************************************************************/

//DLL 接口函数定义
typedef IModuleManageService * (CreateServiceInterface)(UINT uVer);
typedef BOOL (GetServiceInfo)(ServerDllInfoStruct * pServiceInfo, UINT uVer);




//登陆系统
bool CServiceManage::LogonSystem(const TCHAR * szUserName, const TCHAR * szScrPass)
{
	return AfxGetMainWnd()->PostMessage(WM_COMMAND,IDM_GET_ROOM_LIST,0);
}

/********************************************************************************************************/

void CServiceManage::StartAllGames(CListCtrl * pListCtrl)
{
	if (!checkLicense())
	{
		PostQuitMessage(0);
		ExitProcess(0);
		return;
	}
	////注册码检测////
	//bool bCheck = false;
	//bCheck = CheckLicense();
	//if(!bCheck)
	//{
	//	return;
	//}

	//if (m_pIMessage!=NULL) m_pIMessage->ShowMessage(TEXT("启动所有组件的子实例服务!"));//房间信息读取完成"));

	for(int i=0;i<m_ComRoomInfo.GetSize();i++)
	{
	   SC_GameRoomInitStruct * p=(SC_GameRoomInitStruct *)m_ComRoomInfo.GetAt(i);
	   if(!p)continue;

       // PengJiLin, 2011-4-2, 指定哪些游戏是可用的
       if(FALSE == m_GameSelecter.GameIsUsable(p->ServiceInfo.uNameID))
       {
           p->hDllInstance = NULL;
           p->hProcessHandle = NULL;
           memset(&(p->InitInfo), 0, sizeof(p->InitInfo));
           continue;
       }

	#ifdef BZ_LICENSE
		if(coreGetGameLicense(p->ServiceInfo.uNameID)==p->ServiceInfo.uNameID)
	#else
		if (true)
	#endif // BZ_LICENSE
	   {
	   //加载各组件并得到两个接口函数地址
	   if(!bGetServiceInfo(p))
	   {
		   CString strMessage;
		   strMessage.Format(TEXT("房间对应的库%s不存在：%s"),p->ServiceInfo.szDLLFileName,g_Service.GetLastErrorMsg());
		   if (m_pIMessage!=NULL) m_pIMessage->ShowMessage(strMessage);//房间信息读取完成"));
		   //AfxMessageBox(strMessage,MB_ICONSTOP);
		   continue;
	   }
	   if(!StartGameRoom(p))continue;
	   }
	   else
	   {
		   CString s=p->ServiceInfo.szGameName;
		   if(s.Left(7)!="未认证_")
		   {
			   s="未认证_"+s;
			   if(s.GetLength()>60)
				   s=s.Left(60);
		   }	
		   strcpy(p->ServiceInfo.szGameName,s);
	   }
	}
	//UpdateRoomListInfo(pListCtrl);
	if (m_pIMessage!=NULL) m_pIMessage->ShowMessage(TEXT("启动所有组件的子实例服务完成!"));
}


//启动游戏房间
bool CServiceManage::bGetServiceInfo(SC_GameRoomInitStruct * pComRoomInfo)
{
	////注册码检测////
	//bool bCheck = false;
	//bCheck = CheckLicense();
	//if(!bCheck)
	//{
	//	return false;
	//}

	//AfxMessageBox("GetServiceInfo!");

	try
	{
		if ((pComRoomInfo->hDllInstance!=NULL)||(pComRoomInfo->pIManageService!=NULL)) return true;

		#ifdef BZ_LICENSE
			if(coreGetGameLicense(pComRoomInfo->ServiceInfo.uNameID)!=pComRoomInfo->ServiceInfo.uNameID)
		#else
			if (false)
		#endif // BZ_LICENSE
		{
			CString s=pComRoomInfo->ServiceInfo.szGameName;
			if(s.Left(7)!="未认证_")
			{
				s="未认证_"+s;
				if(s.GetLength()>60)
					s=s.Left(60);
			}	
			strcpy(pComRoomInfo->ServiceInfo.szGameName,s);		
			return false;
		}
		//判断文件
		struct stat FileStat;
		if (stat(pComRoomInfo->ServiceInfo.szDLLFileName,&FileStat)!=0) 
			return false;
			//throw TEXT("组件不存在");

		//加载组件
		pComRoomInfo->hDllInstance=AfxLoadLibrary(pComRoomInfo->ServiceInfo.szDLLFileName);
		if (pComRoomInfo->hDllInstance==NULL) 
			return false;//throw TEXT("组件装载失败");

		//获取管理接口
		CreateServiceInterface * pCreateServiceInterface=(CreateServiceInterface *)GetProcAddress(pComRoomInfo->hDllInstance,TEXT("CreateServiceInterface"));
		if (pCreateServiceInterface==NULL) 
			return false;//throw TEXT("非法组件");
		pComRoomInfo->pIManageService=pCreateServiceInterface(DEV_LIB_VER);
		if (pComRoomInfo->pIManageService==NULL)
			return false;//throw TEXT("组件管理接口获取失败，可能是资源不足");


		GetServiceInfo * pGetServiceInfo=(GetServiceInfo *)GetProcAddress(pComRoomInfo->hDllInstance,TEXT("GetServiceInfo"));
		if (pGetServiceInfo==NULL) 
			return false;//throw TEXT("非法组件");

        // PengJiLin, 2011-4-2, 指定哪些游戏是可用的
        ServerDllInfoStruct ServiceDllInfo;
        if(FALSE == pGetServiceInfo(&ServiceDllInfo,DEV_LIB_VER)) return false;
        if(FALSE == m_GameSelecter.GameIsUsable(ServiceDllInfo.uNameID)) return false;

		//效验组件
		///ServerDllInfoStruct ServiceDllInfo;
		//ServerDllInfoStruct * temp;
		//if (pGetServiceInfo(temp,DEV_LIB_VER)==FALSE) throw TEXT("获取组件信息失败");


	}
	catch (...)
	{
		TRACE("CATCH:%s with %s\n",__FILE__,__FUNCTION__);
		return false;
	}

	return true;
}

//启动游戏房间
bool CServiceManage::StartGameRoom(SC_GameRoomInitStruct * pComRoomInfo)
{
	////注册码检测////
	//bool bCheck = false;
	//bCheck = CheckLicense();
	//if(!bCheck)
	//{
	//	return false;
	//}

	try
	{
	
		UINT errCode=0;
		//启动组件
		if (pComRoomInfo->pIManageService->InitService(&pComRoomInfo->InitInfo)==false) throw TEXT("组件初始化错误");
		if (pComRoomInfo->pIManageService->StartService(errCode)==false) throw TEXT("组件启动错误");
		//pListCtrl->setSetItemText(pListCtrl->,3,TEXT("启动"));


		return true;
	}
	catch (TCHAR * szError) { lstrcpy(m_szError,szError); TRACE("CATCH:%s with %s\n",__FILE__,__FUNCTION__);}
	catch (CAFCException * pException)
	{ TRACE("CATCH:%s with %s\n",__FILE__,__FUNCTION__);
		pException->GetErrorMessage(m_szError,sizeof(m_szError));
		pException->Delete();	
	}
	catch (...)	{ lstrcpy(m_szError,TEXT("发生未知异常错误")); TRACE("CATCH:%s with %s\n",__FILE__,__FUNCTION__);}

	//清理资源
	if (pComRoomInfo->pIManageService!=NULL)
	{
		pComRoomInfo->pIManageService->StoptService();
		pComRoomInfo->pIManageService->UnInitService();
		pComRoomInfo->pIManageService->DeleteService();
		pComRoomInfo->pIManageService=NULL;
	}
	if (pComRoomInfo->hDllInstance!=NULL)
	{
		AfxFreeLibrary(pComRoomInfo->hDllInstance);
		pComRoomInfo->hDllInstance=NULL;
	}

	return false;
}

//更新服务列表,启动时调用,将重起所有游戏
bool CServiceManage::UpdateServiceInfo(CListCtrl * pListCtrl)
{
	//读数据库得到房间信息
	GetRoomListInfo();

	//加载各组件并得到两个接口函数地址
	//for(int i = 0;i<=m_ComRoomInfo.GetUpperBound();i++)
	//{
	//	SC_GameRoomInitStruct * p=(SC_GameRoomInitStruct *)m_ComRoomInfo.GetAt(i);
	//	if(!bGetServiceInfo(p))continue;
	//}
	
	//利用接口函数启动各游戏
    StartAllGames(pListCtrl);
	//更新所有房间列表
	UpdateRoomListInfo(pListCtrl);
	return false;
}

//构造函数
CServiceInfo::CServiceInfo(void)
{
	m_uLogonTimes=0;
	m_uModelDlgCount=0;
	m_uSystemState=STS_NO_LOGON;
	memset(m_szName,0,sizeof(m_szName));
	memset(m_szMD5Pass,0,sizeof(m_szMD5Pass));
	memset(m_szServiceIP,0,sizeof(m_szServiceIP));
	memset(m_szServerGUID,0,sizeof(m_szServerGUID));
	memset(m_szSQLName,0,sizeof(m_szSQLName));
	memset(m_szSQLPass,0,sizeof(m_szSQLPass));
	memset(m_szSQLAddr,0,sizeof(m_szSQLAddr));
	memset(m_szLogonAddr,0,sizeof(m_szLogonAddr));

}

//析构函数
CServiceInfo::~CServiceInfo(void)
{
	SaveData();
}

//初始化函数
bool CServiceInfo::LoadData()
{
	CString strBuffer;

	//读取登陆名字
	strBuffer=AfxGetApp()->GetProfileString(TEXT("ServerInfo"),TEXT("Name"),TEXT("Administrator"));
	if (strBuffer.GetLength()>20) strBuffer.GetBufferSetLength(20);
	lstrcpy(m_szName,strBuffer);

	//获取登陆地址
	strBuffer=AfxGetApp()->GetProfileString(TEXT("ServerInfo"),TEXT("LogonAddr"),TEXT("127.0.0.1"));
	if (strBuffer.GetLength()>(sizeof(m_szLogonAddr)-1)) strBuffer.GetBufferSetLength((sizeof(m_szLogonAddr)-1));
	lstrcpy(m_szLogonAddr,strBuffer);

	//读取 GUID
	strBuffer=AfxGetApp()->GetProfileString(TEXT("ServerInfo"),TEXT("GUID"));
	if (strBuffer.GetLength()>36) strBuffer.GetBufferSetLength(36);
	lstrcpy(m_szServerGUID,strBuffer);

	return true;
}

//保存数据
bool CServiceInfo::SaveData()
{
	if (m_szName[0]!=0) AfxGetApp()->WriteProfileString(TEXT("ServerInfo"),TEXT("Name"),m_szName);
	if (m_szServerGUID[0]!=0) AfxGetApp()->WriteProfileString(TEXT("ServerInfo"),TEXT("GUID"),m_szServerGUID);
	if (m_szLogonAddr[0]!=0) AfxGetApp()->WriteProfileString(TEXT("ServerInfo"),TEXT("LogonAddr"),m_szLogonAddr);
	return true;
}

//是否登陆系统
bool CServiceInfo::IsLogonSystem()
{
	bool bLogon=((m_uSystemState==STS_SYSTEM_LOCK)||(m_uSystemState==STS_LOGON_FINISH));
	return bLogon;
}

//是否锁定系统
bool CServiceInfo::IsLockSystem()
{
	bool bLock=(m_uSystemState==STS_SYSTEM_LOCK);
	return bLock;
}

//获取登陆次数
UINT CServiceInfo::GetLogonTimes() 
{ 
	return m_uLogonTimes; 
}

/********************************************************************************************************/


//析构函数
CServiceManage::~CServiceManage(void)
{
	//清理资源
	for (INT_PTR i=0;i<m_ServiceInfo.GetCount();i++)
	{
		SC_ServiceInfoStruct * pServiceInfo=(SC_ServiceInfoStruct *)(SC_ServiceInfoStruct *)m_ServiceInfo.GetAt(i);
		if (pServiceInfo!=NULL)
		{
			delete pServiceInfo;
			m_ServiceInfo.SetAt(i,NULL);
		}
	}
	for (INT_PTR i=0;i<m_ComRoomInfo.GetCount();i++)
	{
		SC_GameRoomInitStruct * pComRoomInfo=(SC_GameRoomInitStruct *)m_ComRoomInfo.GetAt(i);
		if (pComRoomInfo!=NULL)
		{
			if (pComRoomInfo->pIManageService!=NULL)
			{
				pComRoomInfo->pIManageService->StoptService();
				pComRoomInfo->pIManageService->UnInitService();
				pComRoomInfo->pIManageService->DeleteService();
				pComRoomInfo->pIManageService=NULL;

				ClearRoomTWRecode(pComRoomInfo->InitInfo.uRoomID);
			}
			if (pComRoomInfo->hDllInstance!=NULL) 
			{
				AfxFreeLibrary(pComRoomInfo->hDllInstance);
				pComRoomInfo->hDllInstance=NULL;
			}
			delete pComRoomInfo;
			m_ComRoomInfo.SetAt(i,NULL);
		}
	}

	return;
}

//设置信息接口
bool CServiceManage::SetMessageInterface(IMessageShow * pIMessage)
{
	m_pIMessage=pIMessage; 
	return true;
}
//更新组件列表
bool CServiceManage::UpdateComDLLList(CListCtrl * pListCtrl)
{
	
	HANDLE hDatabase=sqlAddConnection("BZGameLocal.BCF");
	if(hDatabase<(HANDLE)0x1000)
	{
		
		return false;
	}

	HANDLE hRecord=sqlSPSetName(hDatabase,"SP_GetGameServerList",true);
	if(hRecord<(HANDLE)0x1000)
	{
		sqlSPClose(hDatabase,hRecord);
		return false;
	}

	sqlSPExec(hDatabase,hRecord,true);
	//写入数据前应清空记录
	pListCtrl->DeleteAllItems();
	m_ServiceInfo.RemoveAll();

	//读取资料
	TCHAR szBuffer[50];

	while(!sqlEndOfFile(hDatabase,hRecord))
	{
		//定义变量
		SC_ServiceInfoStruct * pServiceInfo=new SC_ServiceInfoStruct;
		memset(pServiceInfo,0,sizeof(SC_ServiceInfoStruct));
		m_ServiceInfo.Add(pServiceInfo);

		//读取数据
		sqlGetValue(hDatabase,hRecord,TEXT("ServerInfoID"),pServiceInfo->uServiceID);
		sqlGetValue(hDatabase,hRecord,TEXT("NameID"),pServiceInfo->ServiceInfo.uNameID);
		sqlGetValue(hDatabase,hRecord,TEXT("DeskPeople"),pServiceInfo->ServiceInfo.uDeskPeople);
		sqlGetValue(hDatabase,hRecord,TEXT("SupportType"),pServiceInfo->ServiceInfo.uSupportType);
		sqlGetValue(hDatabase,hRecord,TEXT("ServiceVer"),pServiceInfo->ServiceInfo.uServiceVer);
		sqlGetValue(hDatabase,hRecord,TEXT("GameName"),pServiceInfo->ServiceInfo.szGameName,sizeof(pServiceInfo->ServiceInfo.szGameName));
		sqlGetValue(hDatabase,hRecord,TEXT("GameTable"),pServiceInfo->ServiceInfo.szGameTable,sizeof(pServiceInfo->ServiceInfo.szGameTable));
		sqlGetValue(hDatabase,hRecord,TEXT("DLLFileName"),pServiceInfo->ServiceInfo.szDLLFileName,sizeof(pServiceInfo->ServiceInfo.szDLLFileName));
		//sqlGetValue(hDatabase,hRecord,TEXT("Writer"),pServiceInfo->ServiceInfo.szWriter,sizeof(pServiceInfo->ServiceInfo.szWriter));
		//sqlGetValue(hDatabase,hRecord,TEXT("DllNote"),pServiceInfo->ServiceInfo.szDllNote,sizeof(pServiceInfo->ServiceInfo.szDllNote));

        // PengJiLin, 2011-4-2, 指定哪些游戏是可用的
        if(FALSE == m_GameSelecter.GameIsUsable(pServiceInfo->ServiceInfo.uNameID))
        {
            memset(pServiceInfo,0,sizeof(SC_ServiceInfoStruct));
            sqlMoveNext(hDatabase,hRecord);
            continue;
        }

		//写入数据
		struct stat FileStat;
		int iItem=pListCtrl->InsertItem(pListCtrl->GetItemCount(),ltoa(pServiceInfo->uServiceID,szBuffer,10));
		pListCtrl->SetItem(iItem,1,1,pServiceInfo->ServiceInfo.szGameName,0,0,0,0);
		if (stat(pServiceInfo->ServiceInfo.szDLLFileName,&FileStat)==0) 
		{
			pListCtrl->SetItem(iItem,2,1,TEXT("已安装"),0,0,0,0);
			pServiceInfo->uServiceStation = 1;
		}
		else 
		{
			pListCtrl->SetItem(iItem,2,1,TEXT("未安装"),0,0,0,0);
			pServiceInfo->uServiceStation = 0;
		}
		pListCtrl->SetItem(iItem,3,1,ltoa(pServiceInfo->ServiceInfo.uServiceVer,szBuffer,10),0,0,0,0);
		pListCtrl->SetItem(iItem,4,1,ltoa(pServiceInfo->ServiceInfo.uDeskPeople,szBuffer,10),0,0,0,0);
		pListCtrl->SetItem(iItem,5,1,pServiceInfo->ServiceInfo.szGameTable,0,0,0,0);
		pListCtrl->SetItem(iItem,6,1,pServiceInfo->ServiceInfo.szDLLFileName,0,0,0,0);
		//pListCtrl->SetItem(iItem,7,1,pServiceInfo->ServiceInfo.szDllNote,0,0,0,0);
		pListCtrl->SetItemData(iItem,(DWORD_PTR)pServiceInfo);
		//pList->SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE,LVS_EX_FULLROWSELECT,LVS_EX_FULLROWSELECT);
		//pList->InsertColumn(0,TEXT("组件 ID"),LVCFMT_LEFT,60);
		//pList->InsertColumn(1,TEXT("组件名字"),LVCFMT_LEFT,100);
		//pList->InsertColumn(2,TEXT("组件状态"),LVCFMT_LEFT,80);
		//pList->InsertColumn(3,TEXT("版本号码"),LVCFMT_LEFT,60);
		//pList->InsertColumn(4,TEXT("游戏人数"),LVCFMT_LEFT,60);
		//pList->InsertColumn(5,TEXT("默认信息表"),LVCFMT_LEFT,100);
		//pList->InsertColumn(6,TEXT("模块名字"),LVCFMT_LEFT,150);
		sqlMoveNext(hDatabase,hRecord);
	}

    // PengJiLin, 2011-4-21, 清 TWLoginRecord 相关内容，避免崩溃后无法登陆
    sqlCloseRecord(hDatabase, hRecord);
    sqlSPClose(hDatabase, hRecord);

    hRecord=sqlSPSetName(hDatabase,"SP_ClearTWLoginRecord");
    if(hRecord>=(HANDLE)0x1000)
    {
        sqlSPAddPara(hDatabase,hRecord,"@ServiceName",m_ServiceName,m_ServiceName.GetLength());
        sqlSPExec(hDatabase,hRecord);
        sqlCloseRecord(hDatabase, hRecord);
        sqlSPClose(hDatabase, hRecord);
    }

	sqlCloseConnection(hDatabase);

	return true;

}


//初始化函数
bool CServiceManage::Init()
{
	return true;
}



//停止游戏房间
bool CServiceManage::StopGameRoom(SC_GameRoomInitStruct * pComRoomInfo)
{
	try
	{
		//判断状态
		if ((pComRoomInfo->hDllInstance==NULL)||(pComRoomInfo->pIManageService==NULL)) return true;

		//停止组件
		if (pComRoomInfo->pIManageService->StoptService()==false) throw TEXT("组件停止错误");
		if (pComRoomInfo->pIManageService->UnInitService()==false) throw TEXT("组件卸载错误");
		if (pComRoomInfo->pIManageService->DeleteService()==false) throw TEXT("组件清理错误");
		pComRoomInfo->pIManageService=NULL;

		//卸载组件
		AfxFreeLibrary(pComRoomInfo->hDllInstance);
		pComRoomInfo->hDllInstance=NULL;

		ClearRoomTWRecode(pComRoomInfo->InitInfo.uRoomID);

		return true;
	}
	catch (TCHAR * szError) { lstrcpy(m_szError,szError);TRACE("CATCH:%s with %s\n",__FILE__,__FUNCTION__); }
	catch (...)	{ lstrcpy(m_szError,TEXT("发生未知异常错误")); TRACE("CATCH:%s with %s\n",__FILE__,__FUNCTION__);}

	//清理资源
	if (pComRoomInfo->pIManageService!=NULL)
	{
		pComRoomInfo->pIManageService->StoptService();
		pComRoomInfo->pIManageService->UnInitService();
		pComRoomInfo->pIManageService->DeleteService();
		pComRoomInfo->pIManageService=NULL;

		ClearRoomTWRecode(pComRoomInfo->InitInfo.uRoomID);
	}
	if (pComRoomInfo->hDllInstance!=NULL)
	{
		AfxFreeLibrary(pComRoomInfo->hDllInstance);
		pComRoomInfo->hDllInstance=NULL;
	}

	return false;
}

/********************************************************************************************************/

//网络读取消息
bool CServiceManage::OnSocketReadEvent(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket)
{
	switch (pNetHead->bMainID)
	{
	case MDM_CONNECT:		//连接信息
		{
			switch (pNetHead->bAssistantID)
			{
			case ASS_NET_TEST:			//网络检测
				{
					if (uDataSize!=0) return false;
					pClientSocket->SendData(MDM_CONNECT,ASS_NET_TEST,0);
					return true;
				}
			case ASS_CONNECT_SUCCESS:	//连接成功
				{
					MSG_S_ConnectSuccess * pVerInfo=(MSG_S_ConnectSuccess *)pNetData;

					//版本效验
					if (GAME_MAX_VER<pVerInfo->bLessVer) 
					{
						if (m_pIMessage!=NULL) m_pIMessage->ShowMessage(TEXT("游戏控制器版本太低了，无法继续使用..."));
						pClientSocket->CloseSocket(false);
						return false;
					}

					//发送登陆信息
					MSG_SL_S_LogonByNameInfo MSGLogonInfo;
					lstrcpy(MSGLogonInfo.szName,m_Info.m_szName);
					lstrcpy(MSGLogonInfo.szMD5Pass,m_Info.m_szMD5Pass);
					lstrcpy(MSGLogonInfo.szServerGUID,m_Info.m_szServerGUID);
					pClientSocket->SendData(&MSGLogonInfo,sizeof(MSGLogonInfo),MDM_SL_SERVICE,ASS_SL_LOGON_BY_NAME,0);

					return true;
				}
			}
			return false;
		}
	case MDM_SL_SERVICE:	//服务登陆部分
		{
			switch (pNetHead->bAssistantID)
			{
			case ASS_SL_LOGON_SUCCESS:	//登陆成功
				{
					//效验数据
					if (uDataSize!=sizeof(MSG_SL_R_ConnectInfo)) return false;
					MSG_SL_R_ConnectInfo * pConnectInfo=(MSG_SL_R_ConnectInfo *)pNetData;

					//处理数据
					pClientSocket->CloseSocket(false);
					m_Info.m_uLogonTimes++;
					m_Info.m_uSystemState=STS_LOGON_FINISH;
					lstrcpy(m_Info.m_szSQLName,pConnectInfo->szSQLName);
					lstrcpy(m_Info.m_szSQLPass,pConnectInfo->szSQLPass);
					lstrcpy(m_Info.m_szSQLAddr,pConnectInfo->szSQLAddr);
					lstrcpy(m_Info.m_szServiceIP,pConnectInfo->szServiceIP);
					if (m_pIMessage!=NULL) m_pIMessage->ShowMessage(TEXT("用户信息效验成功，正在读取大厅信息..."));
					AfxGetMainWnd()->PostMessage(WM_COMMAND,IDM_GET_ROOM_LIST,0);

					return true;
				}
			case ASS_SL_LOGON_ERROR:	//登陆失败
				{
					//效验数据
					if (uDataSize!=0) return false;

					//处理数据
					CString strMessage=TEXT("登陆发生未知错误，请检查系统配置...");
					switch (pNetHead->bHandleCode)
					{
					case ERR_SL_GUID_ERROR: 
						{  
							strMessage=TEXT("系统授权号错误，登陆失败...");
							break;	
						}
					case ERR_SL_LOGON_IP_ERROR: 
						{ 
							strMessage=TEXT("此授权号不能在此服务器上登陆，登陆失败...");
							break;	
						}
					case ERR_SL_LOGON_NAME_ERROR: 
						{	
							strMessage=TEXT("登陆名字错误，登陆失败...");
							break;	
						}
					case ERR_SL_LOGON_NAME_LIMIT: 
						{	
							strMessage=TEXT("登陆名已经被禁用，登陆失败...");
							break;	
						}
					case ERR_SL_LOGON_PASS_ERROR: 
						{ 
							strMessage=TEXT("登陆密码错误，登陆失败...");
							break; 
						}
					}
					
					//显示错误信息
					pClientSocket->CloseSocket(false);
					m_Info.m_uSystemState=STS_NO_LOGON;
					if (m_pIMessage!=NULL) m_pIMessage->ShowMessage(strMessage);
					AfxGetMainWnd()->PostMessage(WM_COMMAND,IDM_LOGON_SYSTEM,0);

					return true;
				}
			}
			return false;
		}
	}
	return false;
}

//网络连接消息
bool CServiceManage::OnSocketConnectEvent(UINT uErrorCode, CTCPClientSocket * pClientSocket)
{
	if (uErrorCode!=0)
	{
		if (m_Info.m_uSystemState==STS_LOGON_ING) m_Info.m_uSystemState=STS_NO_LOGON;
		if (m_pIMessage!=NULL) m_pIMessage->ShowMessage(TEXT("登陆服务器没有启动，请尝试其他登陆服务器..."));
		AfxGetMainWnd()->PostMessage(WM_COMMAND,IDM_LOGON_SYSTEM,0);
	}
	return true;
}

//网络关闭消息
bool CServiceManage::OnSocketCloseEvent()
{
	if (m_Info.IsLogonSystem()==false)
	{
		m_Info.m_uSystemState=STS_NO_LOGON;
		if (m_pIMessage!=NULL) m_pIMessage->ShowMessage(TEXT("由于登陆服务器繁忙，请稍后再次登陆..."));
		AfxGetMainWnd()->PostMessage(WM_COMMAND,IDM_LOGON_SYSTEM,0);
	}
	return true;
}

//增加一条组件记录在ServerDllInfo表中，同时把新的组件加到m_ServiceInfo中
bool CServiceManage::SetupCom(CListCtrl * pListCtrl,SC_ServiceInfoStruct pServiceInfo)
{

	HANDLE hDatabase=sqlAddConnection("BZGameLocal.BCF");
	if(hDatabase<(HANDLE)0x1000)
	{
		
		return false;
	}
	HANDLE hRecord=sqlSPSetName(hDatabase,"SP_AddGameServer",true);
	if(hRecord<(HANDLE)0x1000)
	{
		sqlSPClose(hDatabase,hRecord);
		return false;
	}

	sqlSPAddPara(hDatabase,hRecord,"@ServerInfoID",pServiceInfo.uServiceID);
	sqlSPAddPara(hDatabase,hRecord,"@NameID",pServiceInfo.ServiceInfo.uNameID);
	sqlSPAddPara(hDatabase,hRecord,"@DeskPeople",pServiceInfo.ServiceInfo.uDeskPeople);
	sqlSPAddPara(hDatabase,hRecord,"@SuppoerType",pServiceInfo.ServiceInfo.uSupportType);
	sqlSPAddPara(hDatabase,hRecord,"@ServerVer",pServiceInfo.ServiceInfo.uServiceVer);
	sqlSPAddPara(hDatabase,hRecord,"@GameName",pServiceInfo.ServiceInfo.szGameName,sizeof(pServiceInfo.ServiceInfo.szGameName));
	sqlSPAddPara(hDatabase,hRecord,"@GameTable",pServiceInfo.ServiceInfo.szGameTable,sizeof(pServiceInfo.ServiceInfo.szGameTable));
	sqlSPAddPara(hDatabase,hRecord,"@DllFileName",pServiceInfo.ServiceInfo.szDLLFileName,sizeof(pServiceInfo.ServiceInfo.szDLLFileName));

	sqlSPExec(hDatabase,hRecord);

	int ret=sqlSPGetReturnValue(hDatabase,hRecord);
	sqlCloseConnection(hDatabase);

	if(ret==1)
	{
		if (m_pIMessage!=NULL) m_pIMessage->ShowMessage("不能新建和现有组件ServerInfoID相同的组件,请重新填写! :-) ");//AfxMessageBox("不能新建和现有组件ServerInfoID相同的组件,请重新填写! :-) ");
		return false;
	}

	m_ServiceInfo.Add(&pServiceInfo);//同时把新的组件加到m_ServiceInfo中
	UpdateComDLLList(pListCtrl);
	return true;

}

////修改一条组件记录在ServerDllInfo表中
bool CServiceManage::ModifCom(CListCtrl * pListCtrl,SC_ServiceInfoStruct * avantp,SC_ServiceInfoStruct pServiceInfo)
{
	HANDLE hDatabase=sqlAddConnection("BZGameLocal.BCF");
	if(hDatabase<(HANDLE)0x1000)
		return false;
	HANDLE hRecord=sqlSPSetName(hDatabase,"SP_EditGameServer",true);
	if(hRecord<(HANDLE)0x1000)
	{
		sqlSPClose(hDatabase,hRecord);
		return false;
	}


	sqlSPAddPara(hDatabase,hRecord,"@ServerInfoID",pServiceInfo.uServiceID);
	sqlSPAddPara(hDatabase,hRecord,"@NameID",pServiceInfo.ServiceInfo.uNameID);
	sqlSPAddPara(hDatabase,hRecord,"@DeskPeople",pServiceInfo.ServiceInfo.uDeskPeople);
	sqlSPAddPara(hDatabase,hRecord,"@SuppoerType",pServiceInfo.ServiceInfo.uSupportType);
	sqlSPAddPara(hDatabase,hRecord,"@ServerVer",pServiceInfo.ServiceInfo.uServiceVer);
	sqlSPAddPara(hDatabase,hRecord,"@GameName",pServiceInfo.ServiceInfo.szGameName,sizeof(pServiceInfo.ServiceInfo.szGameName));
	sqlSPAddPara(hDatabase,hRecord,"@GameTable",pServiceInfo.ServiceInfo.szGameTable,sizeof(pServiceInfo.ServiceInfo.szGameTable));
	sqlSPAddPara(hDatabase,hRecord,"@DllFileName",pServiceInfo.ServiceInfo.szDLLFileName,sizeof(pServiceInfo.ServiceInfo.szDLLFileName));

	sqlSPExec(hDatabase,hRecord);

	int ret=sqlSPGetReturnValue(hDatabase,hRecord);
	sqlCloseConnection(hDatabase);
	//使无重复ID
	if (ret==1)
	{
		if (m_pIMessage!=NULL) m_pIMessage->ShowMessage("ServerDllInfo 表中无此ServerInfoID相同的组件,请刷新组件! ");
		//AfxMessageBox("ServerDllInfo 表中无此ServerInfoID相同的组件,请刷新组件! ");
		return false;
	}

	for(int i=0;i<= m_ServiceInfo.GetUpperBound();i++)
	{
		SC_ServiceInfoStruct *p;
		p = (SC_ServiceInfoStruct *) m_ServiceInfo.GetAt(i);
		if(p->uServiceID==pServiceInfo.uServiceID)
		{
			m_ServiceInfo.RemoveAt(i);//删除旧的记录(要么?)
			break;
		}
	}
	m_ServiceInfo.Add(&pServiceInfo);//同时把新的组件加到m_ServiceInfo中
	UpdateComDLLList(pListCtrl);
	return true;

}

// 拷贝新的DLL库到安装目录，用于更新和安装新的组件
bool CServiceManage::CopyNewCom(SC_ServiceInfoStruct* pServiceInfo)
{
	if(AfxMessageBox("拷贝新的动态库文件到WLocalService.exe所在的目录下然后点ok将自动启动对应的房间。",IDOK)==IDOK)
		return true;
	return false;
}

//移出全部组件列表从m_ServiceInfo,删除数据库ServerDLLInfo里的记录
bool CServiceManage::RemoveDLLList(CListCtrl * pListCtrl)
{
	if(!m_ServiceInfo.IsEmpty())//是否还有记录
		return true;
	while(!m_ServiceInfo.IsEmpty())
	{
		RemoveDLLCom(pListCtrl,(SC_ServiceInfoStruct *)m_ServiceInfo.GetAt(m_ServiceInfo.GetUpperBound()));
	}
	//UpdateComDLLList(pListCtrl);
	return true;
}

//移出指定组件列表从m_ServiceInfo并删除其数据库ServerDLLInfo里的记录
bool CServiceManage::RemoveDLLCom(CListCtrl *pListCtrl,SC_ServiceInfoStruct *pServiceInfo)
{
	
	if(m_ServiceInfo.IsEmpty())//是否还有记录
	{
		lstrcpy(m_szError,TEXT("组件列表记录里已没有任何组件"));
		return false;
	}
	//搜索要删除的记录
	for(int i=0;i<= m_ServiceInfo.GetUpperBound();i++)
	{
		SC_ServiceInfoStruct* ptem;
		ptem = (SC_ServiceInfoStruct *)(m_ServiceInfo.GetAt(i)); 
		
		//找到记录
		if(ptem->uServiceID == pServiceInfo->uServiceID)
		{

			HANDLE hDatabase=sqlAddConnection("BZGameLocal.bcf");
			if(hDatabase>(HANDLE)0x1000)
			{
				TCHAR szSQL[200];
				wsprintf(szSQL,TEXT("DELETE FROM TGameServerInfo WHERE ServerInfoID = %d"), pServiceInfo->uServiceID);
				sqlExec(hDatabase,szSQL);
			}
			m_ServiceInfo.RemoveAt(i);//同时移出队列中的记录
				pListCtrl->DeleteItem(i);
			return true;
		}
	}
	//没有记录
	lstrcpy(m_szError,TEXT("列表中已无此组件记录"));
	return false;
}


// 删除数据库房间信息 
bool CServiceManage::DeleteGameRoomInfo(SC_GameRoomInitStruct * pComRoomInfo)
{
	try
	{

		HANDLE hDatabase=sqlAddConnection("BZGameLocal.bcf");
		if(hDatabase<(HANDLE)0x1000)
		{
			
			return false;
		}
		HANDLE hRecord=sqlSPSetName(hDatabase,"SP_EraseRoomInfo");
		if(hRecord<(HANDLE)0x1000)
		{
			sqlSPClose(hDatabase,hRecord);
			return false;
		}

		sqlSPAddPara(hDatabase,hRecord,"@RoomID",pComRoomInfo->InitInfo.uRoomID);

		sqlSPExec(hDatabase,hRecord);

		sqlCloseConnection(hDatabase);

		SC_GameRoomInitStruct *p;
		for(int i = 0;i<=m_ComRoomInfo.GetUpperBound();i++)
		{
			p=(SC_GameRoomInitStruct *)m_ComRoomInfo.GetAt(i);
			if(p->uServiceID==pComRoomInfo->uServiceID && p->InitInfo.szGameRoomName==pComRoomInfo->InitInfo.szGameRoomName)
			{
				m_ComRoomInfo.RemoveAt(i);
				break;
			}
		}

		if (m_pIMessage!=NULL) m_pIMessage->ShowMessage(TEXT("删除组件的子实例信息完成！"));
		return true;
	}
	catch (...) 
	{ TRACE("CATCH:%s with %s\n",__FILE__,__FUNCTION__);
		if (m_pIMessage!=NULL) m_pIMessage->ShowMessage(TEXT("删除组件的子实例信息发生错误，稍后再次尝试..."));
	}
	return false;
}

///desc：获得所有游戏房间信息//W_0002.2
bool CServiceManage::GetRoomListInfo()
{
	////注册码检测////
	//bool bCheck = false;
	//bCheck = CheckLicense();
	//if(!bCheck)
	//{
	//	return false;
	//}

	//AfxMessageBox("GetRoomListInfo!");

	HANDLE hDatabase=sqlAddConnection("BZGameLocal.BCF");
	if(hDatabase<(HANDLE)0x1000)
		return false;
	HANDLE hRecord=sqlSPSetName(hDatabase,"SP_GetRoomList");
	if(hRecord<(HANDLE)0x1000)
		return false;

	try
	{

		sqlSPAddPara(hDatabase,hRecord,"@ServiceName",m_ServiceName,m_ServiceName.GetLength());
		sqlSPExec(hDatabase,hRecord,true);
		while(!sqlEndOfFile(hDatabase,hRecord))
		{
			bool goOn=false,update=false;
			UINT ui=0,uID=0,uPort=0;//无符号
			__int64 i64 = 0;
			//INT ui=0;//符号
			//定义变量
			SC_GameRoomInitStruct * pGameRoomInitInfo=new SC_GameRoomInitStruct;
			memset(pGameRoomInitInfo,0,sizeof(SC_GameRoomInitStruct));

            // PengJiLin, 2011-4-2, 指定哪些游戏是可用的
            sqlGetValue(hDatabase,hRecord,TEXT("NameID"),ui);
			pGameRoomInitInfo->InitInfo.iGameID = ui;
            if(FALSE == m_GameSelecter.GameIsUsable(ui))
            {
                sqlMoveNext(hDatabase,hRecord);
                delete pGameRoomInitInfo;
                continue;
            }

			//已经读取过的不加
			sqlGetValue(hDatabase,hRecord,TEXT("ServerInfoID"),uID);
			sqlGetValue(hDatabase,hRecord,TEXT("SocketPort"),uPort);
			for(int i=0;i<m_ComRoomInfo.GetCount();i++)
			{
				SC_GameRoomInitStruct * p;
				p = (SC_GameRoomInitStruct *)m_ComRoomInfo.GetAt(i);
				//启动的房间
				if(p->uServiceID==uID&&p->InitInfo.uListenPort==uPort&&p->hDllInstance!=NULL&&p->pIManageService!=NULL)
				{

					goOn=true;
					break;
				}
				//停止的房间
				if(p->uServiceID==uID&&p->InitInfo.uListenPort==uPort&&p->hDllInstance==NULL&&p->pIManageService==NULL)
				{
					update=true;
					delete pGameRoomInitInfo;
					pGameRoomInitInfo=p;
					break;
				}
			}
			if(goOn)
			{
				//移动下一条
				sqlMoveNext(hDatabase,hRecord);
				continue;
			}
			if(!update)
				m_ComRoomInfo.Add(pGameRoomInitInfo);
			//读取附加信息
			sqlGetValue(hDatabase,hRecord,TEXT("IDSort"),ui);
			pGameRoomInitInfo->uSortID=ui;
			sqlGetValue(hDatabase,hRecord,TEXT("KindID"),ui);
			pGameRoomInitInfo->uKindID=ui;
			sqlGetValue(hDatabase,hRecord,TEXT("EnableRoom"),pGameRoomInitInfo->bEnable);

			sqlGetValue(hDatabase,hRecord,TEXT("ServerInfoID"),ui);

			pGameRoomInitInfo->uServiceID=ui;////////////////
			//sqlGetValue(hDatabase,hRecord,TEXT("Note"),pGameRoomInitInfo->szRoomNote,sizeof(pGameRoomInitInfo->szRoomNote));
			//读取组件信息
			sqlGetValue(hDatabase,hRecord,TEXT("NameID"),ui);
			pGameRoomInitInfo->ServiceInfo.uNameID=ui;////////////
			sqlGetValue(hDatabase,hRecord,TEXT("DeskPeople"),ui);
			pGameRoomInitInfo->ServiceInfo.uDeskPeople=ui;
			sqlGetValue(hDatabase,hRecord,TEXT("SupportType"),ui);
			pGameRoomInitInfo->ServiceInfo.uSupportType=ui;
			sqlGetValue(hDatabase,hRecord,TEXT("ServiceVer"),ui);
			pGameRoomInitInfo->ServiceInfo.uServiceVer=ui;
			sqlGetValue(hDatabase,hRecord,TEXT("GameName"),pGameRoomInitInfo->ServiceInfo.szGameName,sizeof(pGameRoomInitInfo->ServiceInfo.szGameName));
			sqlGetValue(hDatabase,hRecord,TEXT("GameTable"),pGameRoomInitInfo->ServiceInfo.szGameTable,sizeof(pGameRoomInitInfo->ServiceInfo.szGameTable));
			sqlGetValue(hDatabase,hRecord,TEXT("DLLFileName"),pGameRoomInitInfo->ServiceInfo.szDLLFileName,sizeof(pGameRoomInitInfo->ServiceInfo.szDLLFileName));
			//sqlGetValue(hDatabase,hRecord,TEXT("Writer"),pGameRoomInitInfo->ServiceInfo.szWriter,sizeof(pGameRoomInitInfo->ServiceInfo.szWriter));
			//sqlGetValue(hDatabase,hRecord,TEXT("DllNote"),pGameRoomInitInfo->ServiceInfo.szDllNote,sizeof(pGameRoomInitInfo->ServiceInfo.szDllNote));

			//读取房间信息
			sqlGetValue(hDatabase,hRecord,TEXT("RoomRule"),ui);
			pGameRoomInitInfo->InitInfo.dwRoomRule=ui;
			sqlGetValue(hDatabase,hRecord,TEXT("UserPower"),ui);
			pGameRoomInitInfo->InitInfo.dwUserPower=ui;
			sqlGetValue(hDatabase,hRecord,TEXT("GameTypeID"),ui);
			pGameRoomInitInfo->InitInfo.uComType=ui;
			sqlGetValue(hDatabase,hRecord,TEXT("BasePoint"),ui);
			pGameRoomInitInfo->InitInfo.uBasePoint=ui;
			sqlGetValue(hDatabase,hRecord,TEXT("LessPoint"),ui);
			pGameRoomInitInfo->InitInfo.uLessPoint=ui;
			sqlGetValue(hDatabase,hRecord,TEXT("MaxPoint"),ui);
			pGameRoomInitInfo->InitInfo.uMaxPoint=ui;
			sqlGetValue(hDatabase,hRecord,TEXT("MaxLimite"),ui);
			pGameRoomInitInfo->InitInfo.uMaxLimite=ui;
			sqlGetValue(hDatabase,hRecord,TEXT("RunPublish"),ui);
			pGameRoomInitInfo->InitInfo.uRunPublish=ui;				
			sqlGetValue(hDatabase,hRecord,TEXT("Tax"),ui);
			pGameRoomInitInfo->InitInfo.uTax=ui;
			sqlGetValue(hDatabase,hRecord,TEXT("SocketPort"),ui);
			pGameRoomInitInfo->InitInfo.uListenPort=ui;
			sqlGetValue(hDatabase,hRecord,TEXT("MaxPeople"),ui);
			pGameRoomInitInfo->InitInfo.uMaxPeople=ui;
			pGameRoomInitInfo->InitInfo.iSocketSecretKey = SECRET_KEY;
			/////////////////////////////////////////////////////////
			/////////Kylin 20090116 Vip登陆功能
			sqlGetValue(hDatabase,hRecord,TEXT("VipResever"),ui);
			pGameRoomInitInfo->InitInfo.uVipReseverCount=ui;
			/////////////////////////////////////////////////////////
			//////////////////////////////////////////////////////////////////////////
			//20081211 , Fred Huang  Start
			sqlGetValue(hDatabase,hRecord,TEXT("VirtualUser"),ui);
			pGameRoomInitInfo->InitInfo.uVirtualUser=ui;
			sqlGetValue(hDatabase,hRecord,TEXT("VirtualGameTime"),ui);
			pGameRoomInitInfo->InitInfo.uVirtualGameTime=ui;
			//20081211 , Fred Huang  Stop
			//////////////////////////////////////////////////////////////////////////

			//wushuqun 2009.6.5
			//添加游戏混战场--begin
			sqlGetValue(hDatabase,hRecord,TEXT("BattleRoomID"),ui);
			pGameRoomInitInfo->InitInfo.uBattleRoomID = ui;
			sqlGetValue(hDatabase,hRecord,TEXT("BattleMatchTable"),pGameRoomInitInfo->InitInfo.szBattleGameTable,
				sizeof(pGameRoomInitInfo->InitInfo.szBattleGameTable));
			////////////////--end

			///< 金葫芦二代，是否VIP房间（VIP房间设置密码）
			sqlGetValue(hDatabase,hRecord,TEXT("VIPRoom"),ui);
			pGameRoomInitInfo->InitInfo.bVIPRoom = (ui!=0);

			sqlGetValue(hDatabase,hRecord,TEXT("DeskCount"),ui);
			pGameRoomInitInfo->InitInfo.uDeskCount=ui;
			sqlGetValue(hDatabase,hRecord,TEXT("RoomID"),ui);
			pGameRoomInitInfo->InitInfo.uRoomID=ui;
			sqlGetValue(hDatabase,hRecord,TEXT("StopLogon"),ui);
			pGameRoomInitInfo->InitInfo.uStopLogon=0;
			sqlGetValue(hDatabase,hRecord,TEXT("RoomName"),pGameRoomInitInfo->InitInfo.szGameRoomName,sizeof(pGameRoomInitInfo->InitInfo.szGameRoomName));
			sqlGetValue(hDatabase,hRecord,TEXT("ServerIP"),m_Info.m_szServiceIP,sizeof(m_Info.m_szServiceIP));
			sqlGetValue(hDatabase,hRecord,TEXT("TableLock"),pGameRoomInitInfo->InitInfo.szLockTable,sizeof(pGameRoomInitInfo->InitInfo.szLockTable));
			//wushuqun 2009.6.5
			if (pGameRoomInitInfo->InitInfo.uBattleRoomID > 0)
			{
				memcpy(pGameRoomInitInfo->InitInfo.szGameTable,
					pGameRoomInitInfo->InitInfo.szBattleGameTable,
					sizeof(pGameRoomInitInfo->InitInfo.szBattleGameTable));
			}
			else
			{
				sqlGetValue(hDatabase,hRecord,TEXT("TableMatch"),pGameRoomInitInfo->InitInfo.szGameTable,sizeof(pGameRoomInitInfo->InitInfo.szGameTable));
			}
			////////////////////
			sqlGetValue(hDatabase,hRecord,TEXT("TableIPRule"),pGameRoomInitInfo->InitInfo.szIPRuleTable,sizeof(pGameRoomInitInfo->InitInfo.szIPRuleTable));
			sqlGetValue(hDatabase,hRecord,TEXT("TableUserNameRule"),pGameRoomInitInfo->InitInfo.szNameRuleTable,sizeof(pGameRoomInitInfo->InitInfo.szNameRuleTable));
			//sqlGetValue(hDatabase,hRecord,TEXT("CenterSQLIP"),pGameRoomInitInfo->InitInfo.szSQLIP,sizeof(pGameRoomInitInfo->InitInfo.szSQLIP));
			//sqlGetValue(hDatabase,hRecord,TEXT("LogonSQLIP"),pGameRoomInitInfo->InitInfo.szLogonSQLIP,sizeof(pGameRoomInitInfo->InitInfo.szLogonSQLIP));
			//sqlGetValue(hDatabase,hRecord,TEXT("NativaSQLIP"),pGameRoomInitInfo->InitInfo.szNativeSQLIP,sizeof(pGameRoomInitInfo->InitInfo.szNativeSQLIP));
			// 增加百家乐桌型 add by wlr 20090714

			//{{ Add by lxl 2010-10-08 房间密码
			sqlGetValue(hDatabase,hRecord,TEXT("RoomPassword"),pGameRoomInitInfo->InitInfo.szRoomPassWord,sizeof(pGameRoomInitInfo->InitInfo.szRoomPassWord));
			/// 看是不是百家乐类型的游戏
			char cKey[10];
			CString sPath=CBcfFile::GetAppPath();
			int iResult = 0;
			if (CBcfFile::IsFileExist(sPath + "SpecialRule.bcf"))
			{
				CBcfFile fsr( sPath + "SpecialRule.bcf");
				sprintf(cKey, "%d", pGameRoomInitInfo->ServiceInfo.uNameID);
				iResult = fsr.GetKeyVal (_T("BJLType"), cKey, 0);
			}
			
			if (iResult!=0)
			{
				pGameRoomInitInfo->InitInfo.uDeskType = DESK_TYPE_BJL;
			}
			else
			{
				pGameRoomInitInfo->InitInfo.uDeskType = DESK_TYPE_NORMAL;
			}
			//end of add by wlr 20090714



			//比赛rongqiufen20120106
			if (pGameRoomInitInfo->InitInfo.dwRoomRule & GRR_CONTEST)
			{
				CTime _Now = CTime::GetCurrentTime();
				int nYear = 0, nMonth = 0, nDate = 0, nHour = 0, nMin = 0, nSec = 0, nss = 0;  
				CString temp;
			//	sqlGetValue(hDatabase, hRecord, TEXT("StartTime"), temp);
				if (temp != "")
				{
					sscanf(temp.GetBuffer(), TEXT("%d-%d-%d %d:%d:%d.%d"), &nYear, &nMonth, &nDate, &nHour, &nMin, &nSec, &nss);

					CTime _t(nYear, nMonth, nDate, nHour, nMin, nSec); 
					pGameRoomInitInfo->InitInfo.i64TimeStart = _t.GetTime();
				}
				else
				{
					pGameRoomInitInfo->InitInfo.i64TimeStart = 0;
				}
			//	sqlGetValue(hDatabase, hRecord, TEXT("EndTime"), temp);
				if (temp != "")
				{
					sscanf(temp.GetBuffer(), TEXT("%d-%d-%d %d:%d:%d.%d"), &nYear, &nMonth, &nDate, &nHour, &nMin, &nSec, &nss);

					CTime _t(nYear, nMonth, nDate, nHour, nMin, nSec); 
					pGameRoomInitInfo->InitInfo.i64TimeEnd = _t.GetTime();
				}
				else
				{
					pGameRoomInitInfo->InitInfo.i64TimeEnd = 0;
				}
				
				sqlGetValue(hDatabase,hRecord,TEXT("Chip"), pGameRoomInitInfo->InitInfo.i64Chip);
				sqlGetValue(hDatabase,hRecord,TEXT("LowChip"), pGameRoomInitInfo->InitInfo.i64LowChip);
				sqlGetValue(hDatabase,hRecord,TEXT("LowCount"), ui);
				pGameRoomInitInfo->InitInfo.iLowCount = ui; 
				sqlGetValue(hDatabase,hRecord,TEXT("ContestID"), ui);
				pGameRoomInitInfo->InitInfo.iContestID = ui; 
				sqlGetValue(hDatabase,hRecord,TEXT("Timeout"), ui);
				pGameRoomInitInfo->InitInfo.iTimeout = ui;
				sqlGetValue(hDatabase,hRecord,TEXT("LeastPeople"), ui);
				pGameRoomInitInfo->InitInfo.iLeasePeople = ui;
				sqlGetValue(hDatabase,hRecord,TEXT("UpPeople"), ui);
				pGameRoomInitInfo->InitInfo.iUpPeople = ui;
				pGameRoomInitInfo->InitInfo.uLessPoint = 0;
			}
			//租赁房间ID与时间
			sqlGetValue(hDatabase,hRecord,TEXT("tenancyID"), pGameRoomInitInfo->InitInfo.iLeaseID);
			if (pGameRoomInitInfo->InitInfo.iLeaseID != 0)
			{
				int nYear = 0, nMonth = 0, nDate = 0, nHour = 0, nMin = 0, nSec = 0, nss = 0;  
				CString temp;
				sqlGetValue(hDatabase, hRecord, TEXT("OpenRoomDate"), temp);
				if (temp != "")
				{
					sscanf(temp.GetBuffer(), TEXT("%d-%d-%d %d:%d:%d.%d"), &nYear, &nMonth, &nDate, &nHour, &nMin, &nSec, &nss);

					CTime _t(nYear, nMonth, nDate, nHour, nMin, nSec); 
					pGameRoomInitInfo->InitInfo.i64LeaseTimeStart = _t.GetTime();
				}
				else
				{
					pGameRoomInitInfo->InitInfo.i64LeaseTimeStart = 0;
				}
				int idays = 0;
				sqlGetValue(hDatabase,hRecord,TEXT("AppleyDays"),idays);
				pGameRoomInitInfo->InitInfo.i64LeaseTimeEnd = pGameRoomInitInfo->InitInfo.i64LeaseTimeStart + idays * 24 * 60 * 60;
			}

			//每局时长送金币
			sqlGetValue(hDatabase,hRecord,TEXT("SendTimeLength"), pGameRoomInitInfo->InitInfo.iTimeLength);
			sqlGetValue(hDatabase,hRecord,TEXT("SendCount"), pGameRoomInitInfo->InitInfo.iSendCount);

			sqlMoveNext(hDatabase,hRecord);
		}
		sqlCloseConnection(hDatabase);

		return true;
	}
	catch (...) 
	{ 
		TRACE("CATCH:%s with %s\n",__FILE__,__FUNCTION__);
		if (m_pIMessage!=NULL) 
			m_pIMessage->ShowMessage(TEXT("组件的子实例信息读取发生错误，稍后再次尝试..."));
	}

}

bool CServiceManage::UpdateRoomListInfo(CListCtrl * pListCtrl)
{
	////注册码检测////
	//bool bCheck = false;
	//bCheck = CheckLicense();
	//if(!bCheck)
	//{
	//	return false;
	//}

	SC_GameRoomInitStruct * pGameRoomInitInfo;
	//去除所有记录
	pListCtrl->DeleteAllItems();
	//读取资料
	TCHAR szBuffer[50];
	for(int i=0;i<=m_ComRoomInfo.GetUpperBound();i++)
	{
		pGameRoomInitInfo = (SC_GameRoomInitStruct *) m_ComRoomInfo.GetAt(i);
		int iItem=pListCtrl->InsertItem(pListCtrl->GetItemCount(),ltoa(pGameRoomInitInfo->InitInfo.uRoomID,szBuffer,10));
		pListCtrl->SetItem(iItem,1,1,pGameRoomInitInfo->InitInfo.szGameRoomName,0,0,0,0);
		pListCtrl->SetItem(iItem,2,1,pGameRoomInitInfo->ServiceInfo.szGameName,0,0,0,0);
		if(pGameRoomInitInfo->hDllInstance==NULL||pGameRoomInitInfo->pIManageService==NULL)
			pListCtrl->SetItem(iItem,3,1,TEXT("停止"),0,0,0,0);
		else 
			pListCtrl->SetItem(iItem,3,1,TEXT("启动"),0,0,0,0);
		pListCtrl->SetItem(iItem,4,1,_ltoa(pGameRoomInitInfo->InitInfo.uComType,szBuffer,10),0,0,0,0);
		pListCtrl->SetItem(iItem,5,1,_ltoa(pGameRoomInitInfo->bEnable,szBuffer,10),0,0,0,0);
		pListCtrl->SetItem(iItem,6,1,m_Info.m_szServiceIP,0,0,0,0);
		pListCtrl->SetItem(iItem,7,1,ltoa(pGameRoomInitInfo->InitInfo.uListenPort,szBuffer,10),0,0,0,0);
		pListCtrl->SetItem(iItem,8,1,ltoa(pGameRoomInitInfo->InitInfo.uDeskCount,szBuffer,10),0,0,0,0);
		pListCtrl->SetItem(iItem,9,1,ltoa(0,szBuffer,10),0,0,0,0);
		pListCtrl->SetItem(iItem,10,1,ltoa(pGameRoomInitInfo->InitInfo.uMaxPeople,szBuffer,10),0,0,0,0);
		pListCtrl->SetItem(iItem,11,1,ltoa(pGameRoomInitInfo->InitInfo.uLessPoint,szBuffer,10),0,0,0,0);
		pListCtrl->SetItem(iItem,12,1,ltoa(pGameRoomInitInfo->InitInfo.uBasePoint,szBuffer,10),0,0,0,0);
		pListCtrl->SetItem(iItem,13,1,ltoa(pGameRoomInitInfo->InitInfo.uMaxLimite,szBuffer,10),0,0,0,0);
		pListCtrl->SetItem(iItem,14,1,ltoa(pGameRoomInitInfo->InitInfo.dwRoomRule,szBuffer,10),0,0,0,0);
		pListCtrl->SetItem(iItem,15,1,ltoa(pGameRoomInitInfo->InitInfo.dwUserPower,szBuffer,10),0,0,0,0);
		pListCtrl->SetItem(iItem,16,1,pGameRoomInitInfo->InitInfo.szSQLIP,0,0,0,0);
		pListCtrl->SetItem(iItem,17,1,pGameRoomInitInfo->InitInfo.szLogonSQLIP,0,0,0,0);
		pListCtrl->SetItem(iItem,18,1,pGameRoomInitInfo->InitInfo.szNativeSQLIP,0,0,0,0);
		pListCtrl->SetItem(iItem,19,1,ltoa(pGameRoomInitInfo->uKindID,szBuffer,10),0,0,0,0);
		pListCtrl->SetItem(iItem,20,1,ltoa(pGameRoomInitInfo->ServiceInfo.uNameID,szBuffer,10),0,0,0,0);
		pListCtrl->SetItem(iItem,22,1,pGameRoomInitInfo->InitInfo.szGameTable,0,0,0,0);
		pListCtrl->SetItem(iItem,23,1,pGameRoomInitInfo->InitInfo.szLockTable,0,0,0,0);
		pListCtrl->SetItem(iItem,24,1,pGameRoomInitInfo->InitInfo.szIPRuleTable,0,0,0,0);
		pListCtrl->SetItem(iItem,25,1,pGameRoomInitInfo->InitInfo.szNameRuleTable,0,0,0,0);
		pListCtrl->SetItem(iItem,26,1,ltoa(pGameRoomInitInfo->uSortID,szBuffer,10),0,0,0,0);
		pListCtrl->SetItem(iItem,27,1,ltoa(pGameRoomInitInfo->uServiceID,szBuffer,10),0,0,0,0);
		pListCtrl->SetItem(iItem,28,1,pGameRoomInitInfo->ServiceInfo.szDLLFileName,0,0,0,0);
		pListCtrl->SetItem(iItem,29,1,pGameRoomInitInfo->szRoomNote,0,0,0,0);
		pListCtrl->SetItem(iItem,30,1,ltoa(pGameRoomInitInfo->ServiceInfo.uDeskPeople,szBuffer,10),0,0,0,0);
		if(pGameRoomInitInfo->InitInfo.uStopLogon)
			pListCtrl->SetItem(iItem,31,1,"Yes",0,0,0,0);
		else pListCtrl->SetItem(iItem,31,1,"No",0,0,0,0);
		if(pGameRoomInitInfo->InitInfo.uComType==TY_MONEY_GAME)
			pListCtrl->SetItem(iItem,33,1,ltoa(pGameRoomInitInfo->InitInfo.uTax,szBuffer,10),0,0,0,0);
		else pListCtrl->SetItem(iItem,33,1,"无",0,0,0,0);
		pListCtrl->SetItem(iItem,34,1,ltoa(pGameRoomInitInfo->InitInfo.uRunPublish,szBuffer,10),0,0,0,0);
		pListCtrl->SetItemData(iItem,(DWORD_PTR)pGameRoomInitInfo);		
	}
	return true;
}

void CServiceManage::CountOnLinePeople(CListCtrl * everyDLL, CListCtrl * everyRoom)
{
	long timeStamp=0;
	static int licCount=0;
	if(++licCount>=10)
		licCount=0;
	try
	{
		long licExpires=0;

		HANDLE hDatabase=sqlAddConnection("BZGameLocal.BCF");
		if(hDatabase<(HANDLE)0x1000)
			return;
		if(licCount==0)
			timeStamp=coreGetTimeStamp();
		HANDLE hRecord=sqlSPSetName(hDatabase,"SP_GetRoomList");
		if(hRecord<(HANDLE)0x1000)
			return;


		sqlSPSetName(hDatabase,"SP_GetOnlinePeopleCount");
		sqlSPAddPara(hDatabase,hRecord,"@ServiceName",m_ServiceName,sizeof(m_ServiceName));
		sqlSPExec(hDatabase,hRecord);

		TCHAR szBuffer[50];
		if(licCount==0)
			licExpires=coreGetLicenseExpires();
		while(!sqlEndOfFile(hDatabase,hRecord))
		{
			UINT uServerInfoID=0,uSocketPort=0,uOnLineCount=0;//无符号

			//读取ServerInfoID信息
			sqlGetValue(hDatabase,hRecord,TEXT("ServerInfoID"),uServerInfoID);
			//读取socketPort
			sqlGetValue(hDatabase,hRecord,TEXT("SocketPort"),uSocketPort);
			for(int i=0;i<m_ComRoomInfo.GetCount();i++)
			{
				SC_GameRoomInitStruct * p;
				p = (SC_GameRoomInitStruct *)m_ComRoomInfo.GetAt(i);
				if(uServerInfoID==p->uServiceID&&uSocketPort==p->InitInfo.uListenPort)
				{
					sqlGetValue(hDatabase,hRecord,TEXT("OnLineCount"),uOnLineCount);
					everyRoom->SetItem(i,9,1,ltoa(uOnLineCount,szBuffer,10),0,0,0,0);
				}
			}
			sqlMoveNext(hDatabase,hRecord);
		}
		sqlCloseConnection(hDatabase);

	#ifdef BZ_LICENSE
		
		if(timeStamp>licExpires && licCount==0)//edit..........
		{
			everyDLL->GetParent()->KillTimer(1);
			for(int i=0;i<=m_ComRoomInfo.GetUpperBound();i++)
			{
				SC_GameRoomInitStruct *p;
				p = (SC_GameRoomInitStruct*) m_ComRoomInfo.GetAt(i);
				if(p->hDllInstance==NULL||p->pIManageService==NULL)
					continue;
				StopGameRoom(p);
			}
			//修改提示
			UpdateRoomListInfo(everyRoom);
			
			CString s,code=coreGetCode();
			s.Format("您的服务器未注册或已过期，请与服务商联系。\n\n请将以下机器码发送给服务商，获取注册码文件：\n\n%s\n\n",code);
			everyDLL->GetParent()->MessageBox(s,"提示",MB_ICONERROR);
			everyDLL->GetParent()->GetParentFrame()->DestroyWindow();
			return;
		}

	#endif // BZ_LICENSE

		return;
	}
	catch (...) 
	{ 
		TRACE("CATCH:%s with %s\n",__FILE__,__FUNCTION__);
	}
	return ;
}


CString CServiceManage::GetRoomName()
{
	CString _r = "";
	for(int i=0;i<m_ComRoomInfo.GetCount();i++)
	{
		SC_GameRoomInitStruct * p;
		p = (SC_GameRoomInitStruct *)m_ComRoomInfo.GetAt(i);
		_r += p->InitInfo.szGameRoomName;
		if (i != m_ComRoomInfo.GetCount() - 1)
			_r += "\n\r";
	}
	return _r;
}

void CServiceManage::ClearRoomTWRecode(int iRoomID)
{
	HANDLE hDatabase=sqlAddConnection("BZGameLocal.BCF");
	if(hDatabase<(HANDLE)0x1000)
		return;

	HANDLE hRecord=sqlSPSetName(hDatabase,"SP_GetGameServerList",true);
	if(hRecord<(HANDLE)0x1000)
		return;

	sqlCloseRecord(hDatabase, hRecord);
    sqlSPClose(hDatabase, hRecord);

    hRecord=sqlSPSetName(hDatabase,"SP_ClearRoomTWLoginRecord");
    if(hRecord>=(HANDLE)0x1000)
    {
		sqlSPAddPara(hDatabase,hRecord,"@RoomID", iRoomID);
        sqlSPExec(hDatabase,hRecord);
        sqlCloseRecord(hDatabase, hRecord);
        sqlSPClose(hDatabase, hRecord);
    }
	sqlCloseConnection(hDatabase);
}