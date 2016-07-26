#include "StdAfx.h"
#include "CenterServerManage.h"
#include "CommonUse.h"

//宏定义
#define IDT_UPDATE_INIFILE						20					//检测数据连接定时器
#define IDT_CHECK_DATA_CONNECT					2					//检测数据连接定时器
extern void OutputFun(char* message);
/******************************************************************************************************/
//启动函数
bool CCenterServerManage::OnStart()
{
	//更新游戏列表操作
	//DL_GP_I_UpdateDataListStruct UpdateList;
	//UpdateList.pGameList=&m_GameList;
	//m_SQLDataManage.PushLine(&UpdateList.DataBaseHead,sizeof(UpdateList),DTK_GP_UPDATE_LIST,0,0);
	//检测数据库连接
	//SetTimer(IDT_CHECK_DATA_CONNECT,15000L);
	SetTimer(IDT_UPDATE_INIFILE,15000L);

	//m_ZServerManager对象在B服务器中无用，B中包含的Z的IP和端口都是存储在GAMEGATE.bcf文件中
	m_ZServerManager.Start();

	return true;
}
//定时器消息
bool CCenterServerManage::OnTimerMessage(UINT uTimerID)
{
	switch (uTimerID)
	{
	case IDT_UPDATE_INIFILE:			//更新游戏列表定时器
		{
			KillTimer(IDT_UPDATE_INIFILE);
			GetINIFile();
			SetTimer(IDT_UPDATE_INIFILE,30000L);
			break;
		}
	default:
		break;
	}

	return true;
}
void CCenterServerManage::GetINIFile()
{
	DWORD cfgHandle=cfgOpenFile("BZGameGate.bcf");
	if(cfgHandle<0x10)
		return;

//	#define len(l1,l2)  ( (l1).GetLength () > (l2 ) ? (l2) : ((l1).GetLength ()))

	CString ss = cfgGetValue(cfgHandle,"GateServer","SerialNo","");//客户端当前版本系列号，和用户端比较不同则要用户去升级
	strncpy(m_msgSendToClient.m_strGameSerialNO,ss,sizeof(m_msgSendToClient.m_strGameSerialNO)-1);
	m_msgSendToClient.m_strGameSerialNO[sizeof(m_msgSendToClient.m_strGameSerialNO)-1] = '\0';

	ss = cfgGetValue(cfgHandle,"GateServer","MainServerAddress","");//主服务器IP地址
	strncpy(m_msgSendToClient.m_strMainserverIPAddr,ss,sizeof(m_msgSendToClient.m_strMainserverIPAddr)-1);
	m_msgSendToClient.m_strMainserverIPAddr[sizeof(m_msgSendToClient.m_strMainserverIPAddr)-1] = '\0';

	m_msgSendToClient.m_iMainserverPort=cfgGetValue(cfgHandle,"GateServer","MainServerPort",6800);

	m_msgSendToClient.m_nEncryptType = cfgGetValue(cfgHandle, "GateServer","EncryType", 2); //平台所采用的加密方式，1位MD5，2位SHA1，默认为2; 2009-5-30 zxj

	ss = cfgGetValue(cfgHandle,"GateServer","WebHomeURL","");//主页WEB地址
	strncpy(m_msgSendToClient.m_strHomeADDR,ss,sizeof(m_msgSendToClient.m_strHomeADDR)-1);
	m_msgSendToClient.m_strHomeADDR[sizeof(m_msgSendToClient.m_strHomeADDR)-1] = '\0';

	ss = cfgGetValue(cfgHandle,"GateServer","WebRootURL","");//网站根路径，在程序中涉及的文件子目录根据这个地址来扩展
	strncpy(m_msgSendToClient.m_strWebRootADDR,ss,sizeof(m_msgSendToClient.m_strWebRootADDR)-1);
	m_msgSendToClient.m_strWebRootADDR[sizeof(m_msgSendToClient.m_strWebRootADDR)-1] = '\0';

	ss = cfgGetValue(cfgHandle,"GateServer","WebHelpURL","");//帮助页WEB地址
	strncpy(m_msgSendToClient.m_strHelpADDR,ss,sizeof(m_msgSendToClient.m_strHelpADDR)-1);
	m_msgSendToClient.m_strHelpADDR[sizeof(m_msgSendToClient.m_strHelpADDR)-1] = '\0';

	ss = cfgGetValue(cfgHandle,"GateServer","DownLoadSetupURL","");//客户端安装程序下载页WEB地址
	strncpy(m_msgSendToClient.m_strDownLoadSetupADDR,ss,sizeof(m_msgSendToClient.m_strDownLoadSetupADDR)-1);
	m_msgSendToClient.m_strDownLoadSetupADDR[sizeof(m_msgSendToClient.m_strDownLoadSetupADDR)-1] = '\0';

	ss = cfgGetValue(cfgHandle,"GateServer","DownLoadUpdatepURL","");//客户端更新程序下载页WEB地址
	strncpy(m_msgSendToClient.m_strDownLoadUpdatepADDR,ss,sizeof(m_msgSendToClient.m_strDownLoadUpdatepADDR)-1);
	m_msgSendToClient.m_strDownLoadUpdatepADDR[sizeof(m_msgSendToClient.m_strDownLoadUpdatepADDR)-1] = '\0';

	ss = cfgGetValue(cfgHandle,"GateServer","RallAddvtisFlashURL","");//客户端大厅FLASH广告下载页WEB地址
	strncpy(m_msgSendToClient.m_strRallAddvtisFlashADDR,ss,sizeof(m_msgSendToClient.m_strRallAddvtisFlashADDR)-1);
	m_msgSendToClient.m_strRallAddvtisFlashADDR[sizeof(m_msgSendToClient.m_strRallAddvtisFlashADDR)-1] = '\0';

	ss = cfgGetValue(cfgHandle,"GateServer","RoomRollWords","欢迎来到红鸟网络游戏世界！");//客户端滚动条广告地址
	
	//wushuqun 2009.6.5
	strncpy(m_msgSendToClient.m_strRoomRollADDR,ss,sizeof(m_msgSendToClient.m_strRoomRollADDR)-1);
	m_msgSendToClient.m_strRoomRollADDR[sizeof(m_msgSendToClient.m_strRoomRollADDR)-1] = '\0';

	m_msgSendToClient.m_nIsUsingIMList = cfgGetValue(cfgHandle,"GateServer","UsingIMList",1);

	//大厅左上角是显示ID号还是魅力值
	m_msgSendToClient.m_nHallInfoShowClass=cfgGetValue(cfgHandle,"GateServer","HallInforShowClass",0);

    m_msgSendToClient.m_is_haveZhuanZhang = cfgGetValue(cfgHandle,"GateServer","IsHaveZhuanZhang",0);
	//m_MainserverList.LoadServerList(cfgHandle);

	cfgClose(cfgHandle);

	GetFunction(); ///< 获取服务器使用的功能

	//CString s=CINIFile::GetAppPath ();
	//CINIFile f( s + "CenterServer.ini");
	//#define len(l1,l2)  ( (l1).GetLength () > (l2 ) ? (l2) : ((l1).GetLength ()))
	//////主服务器端口号
	//s=f.GetKeyVal ("www.BZW.cn","BZW","");//服务器参数模式，其实这个应该从客户端获取，客户端要什么模式的参数，就发什么模式的参数
	//m_msgSendToClient.m_iMainserverPort=f.GetKeyVal (s,"m_iMainserverPort",6800);

	//ss=f.GetKeyVal (s,"SerialNO","");//
	//memcpy(m_msgSendToClient.m_strGameSerialNO,ss,len(ss,20)  );//ss.GetLength ());

	//ss=f.GetKeyVal (s,"m_strMainserverIPAddr","");//
	//memcpy(m_msgSendToClient.m_strMainserverIPAddr,ss,len(ss,20)  );

	//ss=f.GetKeyVal (s,"m_strHomeADDR","");//
	//memcpy(m_msgSendToClient.m_strHomeADDR,ss,len(ss,50)  );

	//ss=f.GetKeyVal (s,"m_strWebRootADDR","");//
	//memcpy(m_msgSendToClient.m_strWebRootADDR,ss,len(ss,50)  );

	//ss=f.GetKeyVal (s,"m_strHelpADDR","");//
	//memcpy(m_msgSendToClient.m_strHelpADDR,ss,len(ss,50)  );

	//ss=f.GetKeyVal (s,"m_strDownLoadSetupADDR","123");////
	//memcpy(m_msgSendToClient.m_strDownLoadSetupADDR,ss,len(ss,50)  );

	//ss=f.GetKeyVal (s,"m_strDownLoadUpdatepADDR","123");//
	//memcpy(m_msgSendToClient.m_strDownLoadUpdatepADDR,ss,len(ss,50)  );

	//ss=f.GetKeyVal (s,"m_strRallAddvtisFlashADDR","123");//
	//memcpy(m_msgSendToClient.m_strRallAddvtisFlashADDR,ss,len(ss,50)  );

	//ss=f.GetKeyVal (s,"m_strRoomRollADDR","欢迎来到福乐游游戏中心！");////客户端滚动条广告地址
	//memcpy(m_msgSendToClient.m_strRoomRollADDR,ss,len(ss,100)  );
	//
	////大厅左上角是显示ID号还是魅力值
	//m_msgSendToClient.m_nHallInfoShowClass=f.GetKeyVal(s,"HallInforShowClass",0);

	//m_MainserverList.ReadINIFile(s);

//	OutputFun("加载配置文件成功！");
}

void CCenterServerManage::GetURL(char *strKey)
{
	CBcfFile f(CBcfFile::GetAppPath() + "BZGameGate.bcf");
	CString strValue;

	if (NULL == strKey)
	{
		strValue = f.GetKeyVal(_T("GateServer"),_T("RoomRollWords"),_T("欢迎来到红鸟棋牌游戏中心！"));///< 客户端滚动条广告地址
		strncpy(m_msgSendToClient.m_strRoomRollADDR,strValue,sizeof(m_msgSendToClient.m_strRoomRollADDR)-1);
		m_msgSendToClient.m_strRoomRollADDR[sizeof(m_msgSendToClient.m_strRoomRollADDR)-1] = '\0';

		strValue = f.GetKeyVal(_T("GateServer"),_T("WebHelpURL"),_T(""));///< 帮助页WEB地址
		strncpy(m_msgSendToClient.m_strHelpADDR,strValue,sizeof(m_msgSendToClient.m_strHelpADDR)-1);
		m_msgSendToClient.m_strHelpADDR[sizeof(m_msgSendToClient.m_strHelpADDR)-1] = '\0';

		strValue = f.GetKeyVal(_T("GateServer"),_T("WebHomeURL"),_T(""));///< 主页WEB地址
		strncpy(m_msgSendToClient.m_strHomeADDR,strValue,sizeof(m_msgSendToClient.m_strHomeADDR)-1);
		m_msgSendToClient.m_strHomeADDR[sizeof(m_msgSendToClient.m_strHomeADDR)-1] = '\0';

		strValue = f.GetKeyVal(_T("GateServer"),_T("WebRootURL"),_T(""));///< 网站根路径
		strncpy(m_msgSendToClient.m_strWebRootADDR,strValue,sizeof(m_msgSendToClient.m_strWebRootADDR)-1);
		m_msgSendToClient.m_strWebRootADDR[sizeof(m_msgSendToClient.m_strWebRootADDR)-1] = '\0';
	}
	else
	{
		strValue = f.GetKeyVal(_T("WebHomeURL"),strKey,_T(""));///< 主页WEB地址
		if (strValue.IsEmpty())
		{
			GetURL(NULL);
			return;
		}
		strncpy(m_msgSendToClient.m_strHomeADDR,strValue,sizeof(m_msgSendToClient.m_strHomeADDR)-1);
		m_msgSendToClient.m_strHomeADDR[sizeof(m_msgSendToClient.m_strHomeADDR)-1] = '\0';

		strValue = f.GetKeyVal(_T("RoomRollWords"),strKey,_T(""));///< 客户端滚动条广告地址
		strncpy(m_msgSendToClient.m_strRoomRollADDR,strValue,sizeof(m_msgSendToClient.m_strRoomRollADDR)-1);
		m_msgSendToClient.m_strRoomRollADDR[sizeof(m_msgSendToClient.m_strRoomRollADDR)-1] = '\0';

		strValue = f.GetKeyVal(_T("WebHelpURL"),strKey,_T(""));///< 帮助页WEB地址
		strncpy(m_msgSendToClient.m_strHelpADDR,strValue,sizeof(m_msgSendToClient.m_strHelpADDR)-1);
		m_msgSendToClient.m_strHelpADDR[sizeof(m_msgSendToClient.m_strHelpADDR)-1] = '\0';

		strValue = f.GetKeyVal(_T("WebRootURL"),strKey,_T(""));///< 网站根路径
		strncpy(m_msgSendToClient.m_strWebRootADDR,strValue,sizeof(m_msgSendToClient.m_strWebRootADDR)-1);
		m_msgSendToClient.m_strWebRootADDR[sizeof(m_msgSendToClient.m_strWebRootADDR)-1] = '\0';
	}
	
}

/// 从Function.bcf中读取功能配置
/// @param void
/// @return void
void CCenterServerManage::GetFunction()
{
	CBcfFile f(CBcfFile::GetAppPath() + "Function.bcf");
	CString strValue;

	strValue = f.GetKeyVal("SpecificID","Available","0"); ///< 金葫芦2代指定ID。
	if (0 != _ttoi(strValue))
	{
		m_msgSendToClient.m_nFunction = 1;
		strValue = f.GetKeyVal("SpecificID","NormalID","60000000,69999999");
		m_msgSendToClient.m_lNomalIDFrom = atol(strValue.Left(strValue.Find(",")+1));
		m_msgSendToClient.m_lNomalIDEnd  = atol(strValue.Right(strValue.GetLength()-strValue.Find(",")-1));
	}

	strValue = f.GetKeyVal("OnlineCoin","Available","0"); ///< 百乐门在线送金币。
	if (0 != _ttoi(strValue))
	{
		m_msgSendToClient.m_nFunction |= 1<<1;
	}

	strValue = f.GetKeyVal("RobotExtend","Available","0"); ///< 响当当添加虚拟玩家。
	if (0 != _ttoi(strValue))
	{
		m_msgSendToClient.m_nFunction |= 2<<1;
	}

    // PengJiLin, 2010-8-17, 配置是否禁止私聊
    strValue = f.GetKeyVal("CommFunc","ForbidSay","0");  // 0 = 没有禁止
    if( 0 != _ttoi(strValue))
    {
        m_msgSendToClient.m_nFunction |= 1<<3;
    }
}

//SOCKET 数据读取////请求游戏全局参数
bool CCenterServerManage::OnSocketRead(NetMessageHead * pNetHead, void * pData, UINT uSize, ULONG uAccessIP, UINT uIndex, DWORD dwHandleID)
{
	if (pNetHead->bMainID==MDM_GP_REQURE_GAME_PARA)	////请求游戏全局参数
	{
		if (0 == uSize)
		{
			GetURL(NULL);
		}
		else
		{
			char * pBuf = (char*)pData;
			GetURL(pBuf);
		}

		//RandAServer();//取消负载均衡sdp20140624
		m_TCPSocket.SendData(uIndex,&m_msgSendToClient,sizeof(CenterServerMsg),
			MDM_GP_REQURE_GAME_PARA,0,0,0);
		return true;
	}
	return false;
}
/******************************************************************************************************/

//构造函数
CCenterServerManage::CCenterServerManage(void) : CBaseMainManage()
{
	GetINIFile();
}
//析构函数 
CCenterServerManage::~CCenterServerManage(void)
{
}
//停止服务
bool CCenterServerManage::OnStop()
{
	KillTimer(TID_UPDATE_LIST);
	KillTimer(IDT_CHECK_DATA_CONNECT);
	m_ZServerManager.Stop();
	return true;
}

///数据管理模块初始化
bool CCenterServerManage::OnInit(ManageInfoStruct * pInitData, KernelInfoStruct * pKernelData)
{
	return m_ZServerManager.Init(pInitData,NULL);
}
///数据管理模块卸载
bool CCenterServerManage::OnUnInit()
{
	return m_ZServerManager.UnInit();
}

//获取信息函数
bool CCenterServerManage::PreInitParameter(ManageInfoStruct * pInitData, KernelInfoStruct * pKernelData)
{
	//设置版本信息
	//pKernelData->bMaxVer=GAME_PLACE_MAX_VER;
	//pKernelData->bLessVer=GAME_PLACE_LESS_VER;

	//设置数据库

	//设置使用数据库
	pKernelData->bLogonDataBase=false;//TRUE;
	pKernelData->bNativeDataBase=false;//TRUE;
	pKernelData->bStartSQLDataBase=false;//TRUE;

	//设置使用网络
	pKernelData->bStartTCPSocket=TRUE;
	pInitData->uListenPort=CENTER_SERVER_PORT;//13025
	pInitData->uMaxPeople=300;
	pInitData->iSocketSecretKey = SECRET_KEY;

	CString s;
	s.Format("中心服务启动成功  Port:%d ",
		CENTER_SERVER_PORT );

	OutputFun(s.GetBuffer ());

	return true;
}
//SOCKET 关闭
bool CCenterServerManage::OnSocketClose(ULONG uAccessIP, UINT uSocketIndex, long int lConnectTime)
{
	return true;
}

void CCenterServerManage::RandAServer()
{
	m_ZServerManager.m_MainserverList.RandAServer (m_msgSendToClient.m_strMainserverIPAddr,
		m_msgSendToClient.m_iMainserverPort);
}

bool CCenterServerManage::CMainserverList::RandAServer(char* ipaddr,long& port)
{
	bool bb=false;
	static int curIndex=0;
	Lock();
	if(!m_List.IsEmpty())
	{
		//Z服务负载均衡，平均分配法
		static int curi=0;
		int count=m_List.GetCount ();//服务器IP列表的总数量
		long t=GetCurrentTime() + rand();//随机选择一个数

		int i=0;//余数，发现取余数比乘百分百，然后取整数简单

		 POSITION pos=m_List.GetHeadPosition ();
		for(int j=0;j<m_List.GetCount ();j++)
		{
			Node* nd=(Node*)m_List.GetNext (pos);
			if(nd)
			{				
				if(i == curi)///得到该随机数的IP发送到客户端
				{
					CString s=nd->IPAddr ;
					port=nd->Port ;
					memcpy(ipaddr,s.GetBuffer(s.GetLength ()),20);
					bb=true;
					break;
				}
			}
			i++;
		}
		//Z服务负载均衡，平均分配法

		curi++;
		if(curi>=m_List.GetCount()) 
			curi=0;
	}
	UnLock();
	return bb;
}


void CCenterServerManage::CMainserverList::clear()
{
	Lock();
	while(!m_List.IsEmpty())
	{
		Node* pdesk = (Node*)m_List.RemoveHead();
		delete pdesk;
	}
	m_List.RemoveAll();
	UnLock();
}

CCenterServerManage::CMainserverList::~CMainserverList()
{
	clear();
}

void CCenterServerManage::CMainserverList::LoadServerList(DWORD cfgHandle)
{
	clear();

	CString ss;
	for(int i=0;i<10;i++)
	{
		ss.Format("M_IPAddr%d",i+1);
		CString ip=cfgGetValue(cfgHandle,"GateServer",ss,"");
		if(ip=="")break;
		Node* nd=new Node();
		nd->IPAddr =ip;
		ss.Format("M_Port%d",i+1);
		nd->Port=cfgGetValue(cfgHandle,"GateServer",ss,0);
		ss.Format("M_id%d",i+1);
		nd->id=cfgGetValue(cfgHandle,"GateServer",ss,0);
		Lock();
		m_List.AddTail (nd);
		UnLock();
	}
}


//void CCenterServerManage::CMainserverList::ReadINIFile(CString TMLkey)
//{
//	clear();
//	CString s=CINIFile::GetAppPath ();
//	CINIFile f( s + "CenterServer.ini");
//	//int count=f.GetKeyVal ("www.TML.cn","MainserverListCount",0);//////主服务器个数
//	for(int i=0;i<10;i++)
//	{
//		CString s;
//		s.Format ("%d",i+1);
//		CString ip=f.GetKeyVal (TMLkey,"M_IPAddr"+s,"");
//		if (ip=="")break;
//		Node* nd=new Node();
//		nd->IPAddr =ip;
//		long port=f.GetKeyVal (TMLkey,"M_Port"+s,0);
//		nd->Port =port;
//		port=f.GetKeyVal (TMLkey,"M_id"+s,0);
//		nd->id  =port;
//		Lock();
//		  m_List.AddTail (nd);
//		UnLock();
//
////原来算法
//		//CString s;
//		//s.Format ("Mainserver%d",i+1);
//		//CString ip=f.GetKeyVal (s,"IPAddr","");
//		//Node* nd=new Node();
//		//nd->IPAddr =ip;
//		//long port=f.GetKeyVal (s,"Port",0);
//		//nd->Port =port;
//		//port=f.GetKeyVal (s,"id",0);
//		//nd->id  =port;
//		//Lock();
//		//  m_List.AddTail (nd);
//		//UnLock();
//
//	}
//
//}

CCenterServerManage::CMainserverList::CMainserverList()
{
	///srand( (unsigned)time( NULL ) );
	///InitializeCriticalSection(&cs);
   /// ReadINIFile();
}
/******************************************************************************************************/

//构造函数CCenterServerManage
CCenterServerModule::CCenterServerModule(void) 
{
	return;
}

//析构函数
CCenterServerModule::~CCenterServerModule(void)
{
	return;
}

//初始化函数 
bool CCenterServerModule::InitService(ManageInfoStruct * pInitData)
{
	try
	{
		return m_LogonManage.Init(pInitData,NULL);
	}
	catch (CAFCException * pException)	
	{ 
		pException->Delete();	
		TRACE("CATCH:%s with %s\n",__FILE__,__FUNCTION__);
	}
	catch (...)	
	{ 
		TRACE("CATCH:%s with %s\n",__FILE__,__FUNCTION__);
	}
	return false;
}

//卸载函数
bool CCenterServerModule::UnInitService()
{
	try
	{
		return m_LogonManage.UnInit();
	}
	catch (CAFCException * pException)	{ pException->Delete();	TRACE("CATCH:%s with %s\n",__FILE__,__FUNCTION__);}
	catch (...) {TRACE("CATCH:%s with %s\n",__FILE__,__FUNCTION__);}
	return false;
}

//开始函数 
bool CCenterServerModule::StartService(UINT &errCode)
{
	try
	{
		return m_LogonManage.Start();
	}
	catch (CAFCException * pException)	{ pException->Delete();TRACE("CATCH:%s with %s\n",__FILE__,__FUNCTION__);	}
	catch (...) {TRACE("CATCH:%s with %s\n",__FILE__,__FUNCTION__);}
	return false;
}

//停止函数 
bool CCenterServerModule::StoptService()
{
	try
	{
		return m_LogonManage.Stop();
	}
	catch (CAFCException * pException)	{ pException->Delete();TRACE("CATCH:%s with %s\n",__FILE__,__FUNCTION__);	}
	catch (...) {TRACE("CATCH:%s with %s\n",__FILE__,__FUNCTION__);}
	return false;
}

//删除函数
bool CCenterServerModule::DeleteService()
{
	try	{ delete this; }
	catch (...) {}
	return true;
}

/******************************************************************************************************/
