#pragma once

#include "AfxTempl.h"
#include "MainManageForZ.h"
#include "GameLogonDT.h"
#include "GameListCtrl.h"
//上传头像
#include "UserCustomLogo.h"
//IM类
#include "IMService.h"
//银行类
#include "BankService4Z.h"

///{{ Added by zxd 20100722  GM管理工具所用头文件
#include "InterfaceForGM.h"
/// Added by zxd 20100722}}
#include "DistriManager.h" // 分布式处理
#include "UserManager.h"
///{{ Added by JianGuankun 2012.5.16  手机短信接口
#include "SmsIf.h"
/// Added by JianGuankun 2012.5.16}}

#include <map>
using namespace std;


//宏定义 
#define TID_UPDATE_LIST							50					//更新列表 ID
#define LS_TIME_OUT								60000L				//列表超时数



// ----------------------------------------------------------------------------
// PengJiLin, 2010-6-3, 增加使用第三方DLL文件的声明。(鸥朴)
// ----------------------------------------------------------------------------

// 回调消息结构定义如下
typedef struct msg_head
{
    unsigned long serial_no;        /// 消息流水号
    int opr_ret;                    /// 操作结果
    unsigned char* data;            /// 消息体
} MSG_CONTEXT_HEAD, *LPMSG_CONTEXT_HEAD;


typedef struct user_info
{
    char chUserName[50];
    char chMD5PSW[50];

    int iUserIndex;
}USER_INFO, *LPUSER_INFO;


// 消息类型
#define MSG_TYPE_LOGON              1       // 登录消息
#define     MSG_OP_LOGON_SUCCESS    0       // 用户登录成功
#define     MSG_OP_LOGON_INFO_ERROR 66537   // 用户名或密码错误
#define     MSG_OP_LOGON_USING      66539   // 用户已登录
#define     MSG_OP_LOGON_NO_USER    66542   // 用户不存在
#define     MSG_OP_LOGON_FORBID     66547   // 用户处于禁用状态
#define     MSG_OP_LOGON_OVERDATE   66551   // 用户已过期

// 消息回调函数接口定义：
typedef void (__stdcall *MSG_CALL)(int msg_type, LPMSG_CONTEXT_HEAD msg_context_ptr);

// 函数指针说明

// 验证接口动态库的license
// bCallDesc：调用方标识，定义如下：
#define KFGAME 66
typedef void (__stdcall *SET_LICENCE_FUNC_PTR)(const char* licence, unsigned char bCallDesc);

// 初始化连接接口
typedef int (__stdcall *INIT_FUNC_PTR)(MSG_CALL msg_func, int thread_num, int conn_num);

// 释放动态库接口
typedef bool (__stdcall *RELEASE_FUNC_PTR)();

// 用户合法性验证接口
typedef void (__stdcall *LOGIN_FUNC_PTR)(const char* szUser, const char* szPwd, int iSerialNo);

// 消息回调函数
void __stdcall MsgHandleCallback(int msg_type, LPMSG_CONTEXT_HEAD msg_context_ptr);


// 鸥朴 DLL 文件名称
#define DLL_FILE_NAME_OPETV     "OpetvInterfaceDll.dll"//"ZylDll.dll"


/******************************************************************************************************/

//数据库处理服务
class CServiceDataBaseHandle : public CDataBaseHandle
{
public:
	CServiceDataBaseHandle(void){};
	//函数定义
private:
	//数据库处理接口
	virtual UINT HandleDataBase(DataBaseLineHead * pSourceData);


	//辅助函数
private:
	//效验字符
	bool CheckChar(TCHAR * szChar, bool bStrCheck);

	//处理函数
private:
	/// {{Added by zxd 20100729用于GM工具
	///GM按玩家信息获取已经操作过的情况，玩家ID和昵称对应关系
	UINT OnGMGetOperatedUser(DataBaseLineHead * pSourceData);
	///GM获取奖励方案
	UINT OnGMGetAwardType(DataBaseLineHead * pSourceData);
	///GM获取道具ID与名称对应关系
	UINT OnGMGetPropInfo(DataBaseLineHead * pSourceData);
	///GM对玩家进行奖惩操作
	UINT OnGMOperateToUser(DataBaseLineHead * pSourceData);
	//GM登陆处理
	UINT OnGMLogon(DataBaseLineHead * pSourceData, UINT uLogonType);
	/// Added by zxd 20100729用于GM工具}}


	//大厅登陆处理
	UINT OnPlaceLogon(DataBaseLineHead * pSourceData, UINT uLogonType);
	//更新游戏列表
	UINT OnUpdateGameList(DataBaseLineHead * pSourceData);
	//检测数据库连接
	UINT OnCheckDataConnect(DataBaseLineHead * pSourceData);
	//
	UINT OnGetRoomListPeoCount(DataBaseLineHead * pSourceData);
	UINT OnMailListGet(DataBaseLineHead * pSourceData);
	UINT OnMailUpdate(DataBaseLineHead * pSourceData);
	
	UINT OnGetLogonPeopCount(DataBaseLineHead * pSourceData);
	//玩家头像ID
	void OnUpdateUserLogoID(DataBaseLineHead *pSourceData);
	//玩家头像ID md5值
	void OnRequestUserLogoMD5(DataBaseLineHead *pSourceData);
	//取好友数量
	void OnDBIMGetUserCount(DataBaseLineHead * pSourceData);
	//服务器启动用户在线标志清0
	void OnClearOnlineFlag();
	//用户下线操作
	void OnUserOffline(DataBaseLineHead * pSourceData);
	//设置好友组
	void OnIMSetUser(DataBaseLineHead * pSourceData);
	//获取用户比赛房间ID
	void OnGetContestRoomID(DataBaseLineHead * pSourceData);
	//获取走马灯信息
	void OnGetMarQueueInform(DataBaseLineHead* pSourceData);
	//取得道具信息
	void OnPropGetList(DataBaseLineHead * pSourceDat);
	void OnPropBuy(DataBaseLineHead * pSourceData);
    void OnPropBuyNew(DataBaseLineHead * pSourceData);  // PengJiLin, 2010-10-13, 商店道具的即时购买功能
	void OnPropGive(DataBaseLineHead *pSourceData);
	void OnPropBigBrd(DataBaseLineHead *pSourceData);
	void OnPropBigBrd_BuyAndUse(DataBaseLineHead *pSourceData);
	void OnUserLockAccount(DataBaseLineHead * pSourceData);	//锁定机器 zxj 2009-11-13
	void OnUserBindMobile(DataBaseLineHead * pSourceData);	//绑定手机
	void OnPropBuyVIP(DataBaseLineHead * pSourceData);  //道具界面购买VIP  add by wyl  11-5-21
	void OnSetUserLogo(DataBaseLineHead * pSourceData);//设置用户头像
	//试图进入密码房间  add by lxl  2010-10-08
	void OnCheckRoomPasswd(DataBaseLineHead *pSourceData);

	/// 检查版本zxd add for check version20100312
	UINT OnCheckVersion(DataBaseLineHead * pSourceData);
	//修改写Z登录记录的时机，确保网络卡的时候，不至于导致用户登录不上 2009-4-30 zxj
	UINT OnWriteUserLogonRecord(DataBaseLineHead * pSourceData);
	
	//获取销售列表控件   add  wyl
	UINT OnGetSellGameList(DataBaseLineHead * pSourceData);
	//检测是否发送消息列表
	UINT OnCheckSellGameList(DataBaseLineHead * pSourceData);

	//修改用户基本资料
	UINT OnUpdateUserInfo(DataBaseLineHead* pSourceData,UINT nType);

	//修改密码
	UINT OnUpdateUserPwd(DataBaseLineHead* pSourceData);

	UINT OnReceiveMoney(DataBaseLineHead * pSourceData);

	UINT OnCharmExchangeList(DataBaseLineHead * pSourceData);

	UINT OnCharmExchange(DataBaseLineHead * pSourceData);

	UINT OnSetFriendType(DataBaseLineHead * pSourceData);

	//查询玩家的ID或昵称
	UINT OnQueryUserNickNameID(DataBaseLineHead* pSourceData);

	//关于银行处理///////////////////

	//打开银行
	UINT OnBankOpen(DataBaseLineHead* pSourceData);

	//查询钱包
	UINT OnBankGetGameWallet(DataBaseLineHead* pSourceData);

	//银行存取
	UINT OnBankCheck(DataBaseLineHead* pSourceData);

	//银行转账
	UINT OnBankTransfer(DataBaseLineHead* pSourceData);

	//查询银行转账记录
	UINT OnBankTransRecord(DataBaseLineHead* pSourceData);

	//存单业务
	UINT OnBankDeposit(DataBaseLineHead* pSourceData);

	//查询存单
	UINT OnBankQueryDeposits(DataBaseLineHead* pSourceData);

	//修改银行密码
	UINT OnBankChangePassword(DataBaseLineHead* pSourceData);

	//获取财务状况
	UINT OnBankGetFinanceInfo(DataBaseLineHead* pSourceData);

	//获取最近登录的游戏
	UINT OnLastLoginGame(DataBaseLineHead* pSourceData);
	
	///zht 2012-3-6
	HANDLE sqlSPSetNameEx(HANDLE & hHandle,LPCTSTR szSPName,bool bReturnValue = false);



};

/******************************************************************************************************/

//游戏登陆管理类
class CGameLogonManage : public CBaseMainManageForZ
///{{ 实现用于GM管理工具的功能接口
	, public IGMFunction
	, public IDistriManagerNetBase
/// }}
{
	/// {{ 引入GM管理工具
private:
	CGMMessageBase					*m_pGmMessageBase;				///< 用于GM消息处理过程
	IDistriManagerBase				*m_pDistriManager;
public:
	/// 实现从IGMFunction继承过来的接口
	/// 向客户端发送数据
	virtual int SendDataByUser(UINT uUserID, BYTE bMainID, BYTE bAssistantID, UINT bHandleCode, DWORD dwHandleID)
	{
		int nSockIndex = GetIndexByID(uUserID);
		if(nSockIndex >= 0)
		{
			return SendData(nSockIndex, bMainID, bAssistantID, bHandleCode, dwHandleID);
		}
		else
		{
			return m_pDistriManager->SendData(uUserID, bMainID, bAssistantID, bHandleCode, dwHandleID);
		}
	}
	virtual int SendDataByUser(UINT uUserID, void * pData, UINT uBufLen, BYTE bMainID, BYTE bAssistantID, UINT bHandleCode, DWORD dwHandleID)
	{
		int nSockIndex = GetIndexByID(uUserID);
		if(nSockIndex >= 0)
		{
			return SendData(nSockIndex, pData, uBufLen, bMainID, bAssistantID, bHandleCode, dwHandleID);
		}
		else
		{
			return m_pDistriManager->SendData(uUserID, pData, uBufLen, bMainID, bAssistantID, bHandleCode, dwHandleID);
		}
	}
	virtual int SendData(UINT uIndex, BYTE bMainID, BYTE bAssistantID, BYTE bHandleCode, DWORD dwHandleID)
	{
		return m_TCPSocket.SendData(uIndex, bMainID, bAssistantID, bHandleCode, dwHandleID);
	}
	virtual int SendData(UINT uIndex, void * pData, UINT uBufLen, BYTE bMainID, BYTE bAssistantID, BYTE bHandleCode, DWORD dwHandleID)
	{
		return m_TCPSocket.SendData(uIndex, pData, uBufLen, bMainID, bAssistantID, bHandleCode, dwHandleID);
	}
	/// 向数据库模块发送访问请求
	virtual bool PushDataLine(DataBaseLineHead * pData, UINT uSize, UINT uHandleKind, UINT uIndex,DWORD dwHandleID)
	{
		return m_SQLDataManage.PushLine(pData, uSize, uHandleKind, uIndex, dwHandleID);
	}
	/// 判断某连接是否还存在
	virtual bool IsConnectID(UINT uIndex, DWORD dwHandleID)
	{
		return m_TCPSocket.IsConnectID(uIndex, dwHandleID);
	};
	/// 向客户端发送广播数据
	virtual int SendBatchData(void * pData, UINT uBufLen, BYTE bMainID, BYTE bAssistantID, BYTE bHandleCode)
	{
		return m_TCPSocket.SendDataBatch(pData, uBufLen, bMainID, bAssistantID, bHandleCode);
	}
	/// 根据用户ID获取Socket序号
	virtual int GetIndexByID(int iUserID)
	{
		for(int i=0; i<m_UserList.GetCount(); i++)
		{
			LPUSER p = (LPUSER)m_UserList.GetAt(i);
			if(p == NULL)
				continue;
			if(p->UserID==iUserID)
			{
				return p->iSocketIndex;
			}
		}
		return -1;
	}

	virtual LPUSER GetUserByID(int iUserID)
	{
		for(int i=0; i<m_UserList.GetCount(); i++)
		{
			LPUSER p = (LPUSER)m_UserList.GetAt(i);
			if(p == NULL)
				continue;
			if(p->UserID==iUserID)
			{
				return p;
			}
		}
		return NULL;
	}


	/// 根据用户Socket序号获取用户信息
	virtual LPUSER GetUserBySockIdx(UINT uSockIdx)
	{
		if (m_UserList.GetCount() == 0)
		{
			return NULL;
		}

		for(int i = 0; i < m_UserList.GetCount(); ++i)
		{
			LPUSER p = (LPUSER)m_UserList.GetAt(i);
			if(!p)
			{
				return NULL;
			}
			if(uSockIdx == p->iSocketIndex)
			{
				return p;
			}
		}

		return NULL;
	}

	///根据
	/// 判断某玩家是否已经登录
	virtual bool IsUserAlreadyLogon(DWORD dwUserID)
	{
		for(int i=0; i<m_UserList.GetCount(); i++)
		{
			LPUSER p = (LPUSER)m_UserList.GetAt(i);
			if(p == NULL)
				continue;
			if(p->UserID==dwUserID)
			{
				return true;
			}
		}
		return false;
	}

	///根据			2012.11.22
	///	判断某玩家是否已经登录（通过用户名）
	virtual bool IsUserAlreadyLogonByName(char * const szUserName)
	{
		if (szUserName == NULL)
		{
			return false;
		}
		for (int i=0; i<m_UserList.GetCount(); i++)
		{
			LPUSER p = (LPUSER)m_UserList.GetAt(i);
			if(p == NULL)
				continue;
			if(strcmp(p->UserName, szUserName) == 0)
			{
				return true;
			}
		}
		return false;
	}
	/// 判断某玩家是否是GM
	virtual bool IsGameMaster(UINT uIndex)
	{
		for(int i=0; i<m_UserList.GetCount(); i++)
		{
			LPUSER p = (LPUSER)m_UserList.GetAt(i);
			if(p == NULL)
				continue;
			if(p->iSocketIndex == (int)uIndex)
			{
				return p->bIsGameManager;
			}
		}
		return false;
	}
	/// 添加玩家登录记录
	virtual bool AddLogonUser(DWORD dwUserID, UINT uIndex, bool bIsGM = false)
	{
		LPUSER p = (LPUSER)new USERS();
		p->iSocketIndex = uIndex;
		p->UserID = dwUserID;
		p->bIsGameManager = bIsGM;
		m_UserList.Add(p);

		MSG_ZDistriInfo info;
		info.dwUserID = dwUserID;
		info.nZID = m_ID;
		m_pDistriManager->SendNormalData(&info, sizeof(info), MDM_ZD_PACKAGE, ASS_ZD_LOGON, HDC_ZD_CONNECT);
		
		return true;
	}
	/// Added by zxd 20100722}}

private:
	void SendRoomListPeoCount();
	//变量定义
public:
	UINT m_nPort;																//登陆服务器端口
	UINT m_uMaxPeople;															//支持最大人数	
	CString m_strSmsContent;													//短讯格式
	CServerGameListManage						m_GameList;						//游戏列表
	CPtrArray									m_UserList;						//用户列表
	///////////////////////////
	////Kylin 20090316 添加防止账户同时登陆
	int m_ID;//ZID
	///////////////////////////

    // PengJiLin, 2010-6-3, 是否是第三方启动，TRUE = 是。(鸥朴)
    // 注意：这里的变量表示，一旦服务器处于第三方启动状态，则一直都是。
    BOOL m_bRunBySoftware;

private:	
	CUserCustomLogo			*m_userCustomLogo;		//头像上传处理类
	CIMService				*m_imService;			//IM处理类
	CBankService4Z          *m_pBankService;		//银行服务处理业类
	ISms					*m_pISms;				//短信息处理接口 Added by JianGuankun 2012.5.16
public:
		CString				m_CustomFacePath;		//用户目录
		CString				m_CustomFacePathUpload;	//用户传目录
		bool				m_bCustomFaceNeedCheck;	//头像是否需要审核
		BOOL				m_bSendSellGameList;	//2011-5-16, 是否需要发送销售游戏列表


		CTalkMessageFilter  m_TalkFilter; //消息过滤系统
		map<int, TMailItem> m_Mails;

	//函数定义
public:
	//构造函数
	CGameLogonManage(void);
	CGameLogonManage(CGameLogonManage&);
	CGameLogonManage & operator = (CGameLogonManage &);
	//析构函数
	virtual ~CGameLogonManage(void);
	//获取游戏列表
	const CServerGameListManage * GetGameList() { return &m_GameList; };

	//功能函数
public:
	//数据管理模块启动
	virtual bool OnStart();
	//数据管理模块关闭
	virtual bool OnStop();

	//服务扩展接口函数
private:
	//获取信息函数
	virtual bool PreInitParameter(ManageInfoStruct * pInitData, KernelInfoStruct * pKernelData);
	//SOCKET 数据读取
	virtual bool OnSocketRead(NetMessageHead * pNetHead, void * pData, UINT uSize, ULONG uAccessIP, UINT uIndex, DWORD dwHandleID);
	//SOCKET 关闭
	virtual bool OnSocketClose(ULONG uAccessIP, UINT uSocketIndex, long int lConnectTime);
	//数据库处理结果
	virtual bool OnDataBaseResult(DataBaseResultLine * pResultData);
	//定时器消息
	virtual bool OnTimerMessage(UINT uTimerID);
	//道具消息
	void OnPropMessage(NetMessageHead * pNetHead, void * pData, UINT uSize, ULONG uAccessIP, UINT uIndex, DWORD dwHandleID);

	/// {{Added by zxd 20100805 获取在线GM列表
	void OnPlayer2Gm(NetMessageHead * pNetHead, void * pData, UINT uSize, ULONG uAccessIP, UINT uIndex, DWORD dwHandleID);
	/// Added by zxd 20100805 获取在线GM列表}}
	
	void GetINIFile();

	HANDLE sqlSPSetNameEx(HANDLE & hHandle,LPCTSTR szSPName,bool bReturnValue=false);



///////////////////////////////
	/// 根据用户ID获取Socket序号
	/// 向客户端发送广播数据

	/// 向客户端发送广播数据
	virtual int SendBatchDataSelf(void * pData, UINT uBufLen, BYTE bMainID, BYTE bAssistantID, UINT bHandleCode)
	{
		return m_TCPSocket.SendDataBatch(pData, uBufLen, bMainID, bAssistantID, bHandleCode);
	}

	virtual int KickUserSocket(DWORD dwUserID)
	{
		return 0;
	}

	virtual int GetZid()
	{
		return m_ID;
	}

	virtual int GetServerPort()
	{
		return m_nPort;
	}
	// B服务器无法在Z服务器列表中找到玩家时，会反馈消息包给发起的Z服务器。这里进行容错处理。
	virtual int SendDataFail(UINT uUserID, void * pData, UINT uBufLen, BYTE bMainID, BYTE bAssistantID, UINT bHandleCode, DWORD dwHandleID);
///////////////////////////////
};

/******************************************************************************************************/

//游戏登陆模块
class CGameLogonModule : public IModuleManageService
{
	//变量定义 
public:
	CGameLogonManage						m_LogonManage;					//登陆数据管理
	CServiceDataBaseHandle					m_DataBaseHandle;				//数据库数据处理模块

	//函数定义
public:
	//构造函数
	CGameLogonModule(void);
	//析构函数
	virtual ~CGameLogonModule(void);

	//接口函数
public:
	//初始化函数
	virtual bool InitService(ManageInfoStruct * pInitData);
	//卸载函数
	virtual bool UnInitService();
	//开始函数
	virtual bool StartService(UINT &errCode);
	//停止函数
	virtual bool StoptService();
	//删除函数
	virtual bool DeleteService();
};

/******************************************************************************************************/
