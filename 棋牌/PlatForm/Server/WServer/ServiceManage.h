#pragma once
#include "AfxTempl.h"
#include "GameSelect.h"

//系统状态
#define	STS_NO_LOGON					0x00000001						//没有登陆
#define	STS_LOGON_ING					0x00000002						//登陆系统中
#define	STS_LOGON_FINISH				0x00000004						//登陆系统完成
#define	STS_SYSTEM_LOCK					0x00000008						//系统锁定

//常量定义
#define SERVER_ACCESS_DATA				"TEXT(AFCService")//TEXT("ServerAccess")			//数据库名字
#define ROOM_EXE_NAME					TEXT("RoomLoader.EXE")			//房间装载名字

/********************************************************************************************/

//组件信息结构
struct SC_ServiceInfoStruct
{
	UINT								uServiceID;						//组件 ID
	UINT								uServiceStation;				//组件状态
	ServerDllInfoStruct					ServiceInfo;					//组件信息
};

//房间启动信息结构
struct SC_GameRoomInitStruct
{
	//进程模式
	UINT								uRunStation;					//运行状态
	HWND								hWndLoader;						//装载窗口
	HANDLE								hProcessHandle;					//进程句柄

	//DLL 模式
	HINSTANCE							hDllInstance;					//组件句柄
	IModuleManageService				* pIManageService;				//服务接口

	//设置信息
	bool								bEnable;						//是否启用
	UINT								uKindID;						//类型 ID
	UINT								uSortID;						//排列 ID
	UINT								uServiceID;						//组件 ID
	TCHAR								szRoomNote[50];					//房间备注
	ManageInfoStruct					InitInfo;						//初始化信息
	ServerDllInfoStruct					ServiceInfo;					//组件信息
};

//数据库信息
struct SC_SQLInfoStruct
{
	TCHAR								szSQLIP[16];					//数据库地址
	TCHAR								szSQLName[61];					//数据库用户名字
	TCHAR								szSQLPass[61];					//数据库密码
};	

/********************************************************************************************/

//信息提示接口
interface IMessageShow
{
	//接口函数
public:
	//显示信息
	virtual bool ShowMessage(const TCHAR * szMessage)=0;
};

/********************************************************************************************/


//服务信息类
class CServiceInfo
{
	//变量定义
public:
	UINT								m_uLogonTimes;					//登陆次数
	UINT								m_uSystemState;					//系统状态
	UINT								m_uModelDlgCount;				//模态对话框数目

	//用户登陆信息
public:
	TCHAR								m_szMD5Pass[50];				//MD5 密码
	TCHAR								m_szName[61];					//用户名字
	TCHAR								m_szServerGUID[37];				//服务器 GUID
	TCHAR								m_szLogonAddr[51];				//登陆服务地址

	//服务器连接信息 
public:
	TCHAR								m_szSQLName[61];				//SQL 名字
	TCHAR								m_szSQLPass[61];				//SQL 密码
	TCHAR								m_szSQLAddr[16];				//SQL 地址
	TCHAR								m_szServiceIP[16];				//服务 IP

	//函数定义
public:
	//构造函数
	CServiceInfo(void);
	//析构函数
	~CServiceInfo(void);

	//功能函数
public:
	//载入函数
	bool LoadData();
	//保存数据
	bool SaveData();
	//是否锁定系统
	bool IsLockSystem();
	//是否登陆系统
	bool IsLogonSystem();
	//获取登陆次数
	UINT GetLogonTimes();
};

/********************************************************************************************/

//服务控制系统
class CServiceManage : public IClientSocketService
{
public:
	bool bGetServiceInfo(SC_GameRoomInitStruct * pComRoomInfo);
	void StartAllGames(CListCtrl * pListCtrl);
	//变量定义
public:
	CString m_ServiceName;
	TCHAR										m_szError[255];					//错误信息
	CServiceInfo								m_Info;							//服务信息
	CPtrArray									m_ServiceInfo;					//组件信息
	CPtrArray									m_ComRoomInfo;					//房间信息
//	CArray<SC_SQLInfoStruct,SC_SQLInfoStruct>	m_SQLInfo;						//数据库信息

	//数据库信息
protected:
	CString										m_strConnect;					//连接字符

    // PengJiLin, 2011-4-2, 指定哪些游戏是可用的
    CGameSelecter                               m_GameSelecter;

	//控件变量
protected:
	IMessageShow								* m_pIMessage;					//信息接口
	//CTCPClientSocket							m_ClientSocket;					//客户 SOCKET

	//函数定义
public:
	//构造函数
	CServiceManage();
	//析构函数
	virtual ~CServiceManage(void);

	//网络接口函数 
protected:
	//网络读取消息
	virtual bool OnSocketReadEvent(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket);//{return true;}
	//网络连接消息
	virtual bool OnSocketConnectEvent(UINT uErrorCode, CTCPClientSocket * pClientSocket);//{return true;}
	//网络关闭消息
	virtual bool OnSocketCloseEvent();//{return true;}

	//设置函数
public:
	//初始化函数
	bool Init();
	//设置信息接口
	bool SetMessageInterface(IMessageShow * pIMessage);

	//功能函数 
public:
	//获取错误信息
	const TCHAR * const GetLastErrorMsg() { return m_szError; }
	//登陆系统
	bool LogonSystem(const TCHAR * szUserName, const TCHAR * szScrPass);
	
	//组件函数
public:
	//更新组件列表，在数据库里读出所有组件信息，把信息加入m_ServiceInfo（先清空）中，更新视图列表
	bool UpdateComDLLList(CListCtrl * pListCtrl);

	//移出全部组件列表，移出全部组件从m_ServiceInfo,删除数据库ServerDLLInfo里的记录
	bool RemoveDLLList(CListCtrl * pListCtrl);

	//移出指定组件列表
	bool RemoveDLLCom(CListCtrl *pListCtrl,SC_ServiceInfoStruct * pServiceInfo);

	////增加一条组件记录在ServerDllInfo表中，同时把新的组件加到m_ServiceInfo中
	bool SetupCom(CListCtrl * pListCtrl,SC_ServiceInfoStruct pServiceInfo);

	////修改一条组件记录在ServerDllInfo表中,avantp 是修改前的资料,pServiceInfo是修改后的资料
	bool ModifCom(CListCtrl * pListCtrl,SC_ServiceInfoStruct * avantp,SC_ServiceInfoStruct pServiceInfo);

	//房间函数
public:
	//更新房间服务列表,启动时调用,将重起所有游戏
	bool UpdateServiceInfo(CListCtrl * pListCtrl);
	
	//配置游戏房间
	//SC_GameRoomInitStruct * ConfigGameRoom(SC_ServiceInfoStruct * pServiceInfo);

	//启动游戏房间
	bool StartGameRoom(SC_GameRoomInitStruct * pComRoomInfo);

	//停止游戏房间
	bool StopGameRoom(SC_GameRoomInitStruct * pComRoomInfo);

	//更新房间列表
	bool UpdateRoomListInfo(CListCtrl * pListCtrl);

	//得到所有房间信息
	bool GetRoomListInfo();

	// 删除数据库房间信息 
	bool DeleteGameRoomInfo(SC_GameRoomInitStruct * pComRoomInfo);

	//注册码检测
	//bool CheckLicense();
public:

	// 拷贝新的组件，用于更新和安装新的组件
	bool CopyNewCom(SC_ServiceInfoStruct* pServiceInfo);
	//刷新在线人数
	void CountOnLinePeople(CListCtrl * everyDLL, CListCtrl * everyRoom);

	

	CString GetRoomName();

	void ClearRoomTWRecode(int iRoomID);
	
	
};

/********************************************************************************************/
