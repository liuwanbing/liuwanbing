////////////////////////////////////////////////////////////////////////////////
// CLoveSendClassForEXE

// 说明：
// 用于平台代替CLoveSendClass在ICO形式的客户端中的行为
// EXE客户端程序用IPC方式与本类进行信息交互
// 客户端平台与服务器的通讯由本类完成
// 创建者：		曾宪冬
// 创建日期：	2009-02-23

#pragma once

#include "IPC\common.h"
#include "IPC\Sink.h"
#include "IPC\ChannelService.h"

#include <list>

#define TIMER_HEART_BEAT		100		// 用来维持IPC心跳的定时器
const int HEART_BEAT_SECOND		= 3;	// 心跳间隔
const int HEART_BEAT_DELAY		= 1;	// 检测心跳延时

// 用户获赠金币
struct CM_UserState_Send_Times_Money_For_Exe
{
	int dwUserID;
    int                                 dwGetMoney;             // 获赠的金币数
    int                                 dwMoneyOnTimes;         // 设置多少时长赠送的金币
    int                                 dwMoneyOnCounts;        // 设置多少局赠送的金币
    int                                 dwTimesNeed;            // 下一次差多少时长
    int                                 dwCountsNeed;           // 下一次差多少局
};
//用户状态改变
struct CM_UserState_Change_ForExe
{
	BYTE								bActionCode;			//动作代码
	BYTE								bDeskStation;			//桌子位置
	UserItemStruct						uisUserItem;			//用户指针
};

struct GameInfoStructInExe
{
	GameInfoStructInExe()
	{
		bDeskOnwer = true;							//是否台主
		bGameStation = 0;						//游戏状态
		memset(&uisMeUserInfo, 0, sizeof(UserItemStruct));			//玩家信息
		_tcscpy(uisMeUserInfo.GameUserInfo.nickName, TEXT("张三"));
		_tcscpy(uisMeUserInfo.GameUserInfo.szName, TEXT("张三丰"));
		uisMeUserInfo.GameUserInfo.dwUserID = 100001;
		uisMeUserInfo.GameUserInfo.bDeskNO = 3;
		uisMeUserInfo.GameUserInfo.bDeskStation = 1;

		dwGamePower = 0;						//用户权限
		dwMasterPower = 0;						//管理权限
		dwRoomRule = 0;							//设置规则
		pOrderName = NULL;						//等级函数

		//设置参数
		bEnableSound = true;						//允许声音
		bEnableWatch = false;						//允许旁观
		bShowUserInfo = true;						//显示进出
		bEnableBackSound = true;					//背景音乐

		//大厅数据
		_tcscpy(szProcessName, TEXT("10901800.exe"));					//进程名字
		uDeskPeople = 180;						//游戏人数
		uRoomID = 2;							//房间表示
		dwGameMSVer = 2;						//主版本号
		dwGameLSVer = 0;						//副版本号
		uComType = 1;							//游戏类型
		uNameID = 10901800;							//名字表示
		//uNameID = 10003300;							//名字表示
		_tcscpy(szGameName, TEXT("斗地主"));		//游戏名字
		uLessPoint = 0;							//最少经验值
		memset(szGameRoomName, 0, sizeof(szGameRoomName));

		iBasePoint = 1;
		iLowCount = 0;
		i64Chip = 0;
		i64TimeStart = 0;
		i64TimeEnd = 0;
		i64LowChip = 0;
	}
	//IFindUserInterface				* pIFindUser;						//查找接口
	//共享信息
	bool							bDeskOnwer;							//是否台主
	BYTE							bGameStation;						//游戏状态
	UserItemStruct					uisMeUserInfo;						//玩家信息
	long int						dwGamePower;						//用户权限
	long int						dwMasterPower;						//管理权限
	long int						dwRoomRule;							//设置规则
	void							*pOrderName;						//等级函数

	//设置参数
	bool							bEnableSound;						//允许声音
	bool							bEnableWatch;						//允许旁观
	bool							bShowUserInfo;						//显示进出
	bool							bEnableBackSound;					//背景音乐

	//大厅数据
	TCHAR							szProcessName[31];					//进程名字
	UINT							uDeskPeople;						//游戏人数
	UINT							uRoomID;							//房间表示
	DWORD							dwGameMSVer;						//主版本号
	DWORD							dwGameLSVer;						//副版本号
	UINT							uComType;							//游戏类型
	UINT							uNameID;							//名字表示
	TCHAR							szGameName[61];						//游戏名字
	UINT							uLessPoint;							//最少经验值

	bool                            bIsInRecord;                        //是否在录像中 yjj  090223


	int								iBasePoint;
	//比赛专用
	int									iLowCount;
	__int64								i64Chip;
	__int64								i64TimeStart;
	__int64								i64TimeEnd;
	__int64								i64LowChip;

	char						szGameRoomName[61];					//游戏房间名称
};


class CLoveSendClassForEXE : public CDialog, public IFrameInterface, public IChannelMessageSink
{
	struct MessageToSendStruct
	{
		WORD ipcMain;
		WORD ipcSub;
		BYTE *pBuffer;
		WORD wSize;
	};

	//CRITICAL_SECTION m_csForIPCMessage;

	DECLARE_DYNAMIC(CLoveSendClassForEXE)
public:
	CLoveSendClassForEXE(void);
	virtual ~CLoveSendClassForEXE(void);

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg int OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	void OnCancel();
	//------------------------------------------------------------------------\\
								功能函数
public:
	//获取状态参数
	BYTE GetStationParameter() { return m_pGameInfo->bGameStation; };
	//获取自己信息
	UserInfoStruct * GetMeUserInfo() { return &m_pGameInfo->pMeUserInfo->GameUserInfo; };

	//发送网络消息函数
	int SendGameData(BYTE bMainID, BYTE bAssistantID, BYTE bHandleCode);
	//发送网络消息函数
	int SendGameData(void * pData, UINT uBufLen, BYTE bMainID, BYTE bAssistantID, BYTE bHandleCode);
	//框架函数
	virtual bool OnFrameMessage(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket);
	////信息函数
	//virtual bool OnSystemMessage(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket);
	////经验盒消息处理函数
	//virtual bool HandlePointMessage(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket);
	//道具消息
	virtual bool OnPropMessage(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket);
	//使用道具
	virtual bool OnUsePropResult(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket);
	//设置状态参数
	void SetStationParameter(BYTE bGameStation) { m_pGameInfo->bGameStation=bGameStation; };

	//------------------------------------------------------------------------//

	//------------------------------------------------------------------------\\
					从IFrameInterface接口继承的纯虚函数
public:
	//初始化（首次生成使用）
	virtual bool InitFrame(GameInfoStruct * pGameInfo, CWnd * pGameRoom);
	//建立窗口
	virtual bool AFCCreateFrame();
	//关闭窗口
	virtual bool AFCCloseFrame();
	//显示窗口
	virtual void AFCShowFrame(int nShow);
	//发送windows消息
	virtual LRESULT SendWindowMessage(UINT msg,WPARAM wParam,LPARAM lParam);
	//删除对象
	virtual void DeleteFrame();
	//控制消息 
	virtual UINT OnControlMessage(UINT uControlCode, void * pControlData, UINT uDataSize);
	//网络读取消息
	virtual bool OnSocketReadEvent(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket);
	//发送游戏房间消息
	virtual void ShowMsgInGame(TCHAR * szMsg, BYTE bNewOrSys=0,UINT uSize=0, TCHAR * szFontName=NULL,TCHAR *Title=NULL);
	//发送嘟嘟消息
	virtual void ShowMsgDudu(TCHAR * szSendName, TCHAR * szTargetName,TCHAR * szCharString, COLORREF crTextColor, bool bShowTime, UINT uSize=0, TCHAR * szFontName=NULL,int iDuduType=0);
	//发送聊天消息
	virtual void SendShortMessage(MSG_GR_RS_NormalTalk *pShortMessage);
	//录像系统界面设置。
	virtual void SetRecordView() {};
	virtual bool GetGameIsStarting(){return true;}
	//------------------------------------------------------------------------//


	//========================================================================\\
									IPC支持
	//初始化IPC
	bool InitIPC();
	//销毁IPC
	bool DestroyIPC();
	//回调接口
	virtual bool __cdecl OnChannelMessage(const IPC_Head * pHead, const void * pIPCBuffer, WORD wDataSize, HWND hWndSend);
	//IPC内核命令
	bool OnIPCKernel(const IPC_Head * pHead, const void * pIPCBuffer, WORD wDataSize, HWND hWndSend);
	//网络指令
	bool OnIPCSocket(const IPC_Head * pHead, const void * pIPCBuffer, WORD wDataSize, HWND hWndSend);
	//EXE客户端请求用户清单
	bool OnIPCUserInfo(const IPC_Head * pHead, const void * pIPCBuffer, WORD wDataSize, HWND hWndSend);
	//用户使用道具
	bool OnIPCUseProp(const IPC_Head * pHead, const void * pIPCBuffer, WORD wDataSize, HWND hWndSend);
	// 发送IPC消息
	bool SendIPCMessage(void * pIPCBuffer = NULL, WORD wDataSize = 0);
public:
	afx_msg BOOL OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct);
	//========================================================================//


protected:
	int		StartGameClient(void);	// 启动EXE程序
	int		StopGameClient(void);	// 停止EXE程序

private:
	void TranslateGameInfo(void);		// 将m_pGameInfo的内容转移到m_GameInfo中，以便发往EXE进程
	void FlushMessage(void);			// 将消息队列里的内容先发出去
	void SendGlbData(void);				// 发送Glb()里的内容
	void SendBringToTopData(void);		// 通知游戏窗口，弹出到最前面
	void SendPlayerData(UserItemStruct *pUserInfo);			// 更新同桌玩家数据
	void ExeClientShutDown(void);		// 客户端主动关闭

private:
	//========================================================================\\
									IPC支持
	tagShareMemory*					m_pShareMemory;
	char							m_szShareName[64];
	HANDLE							m_hShareMemory;
	HWND							m_hWndChannel;
	PROCESS_INFORMATION				m_GameProcessInfo;				//进程信息
	CIPCRecvCopyData				m_IPCRecvCopyData;
	DWORD							m_dwIPCHeartBeatTick;				// 用来记录心跳消息接收时间的Tick数
	//========================================================================//

	//========================================================================\\
	//游戏数据
private:
	GameInfoStruct						* m_pGameInfo;					//游戏信息指针
	GameInfoStructInExe					m_GameInfo;						//用于与EXE客户端通讯的数据结构，内容从m_pGameInfo而来
	CWnd								* m_pGameRoom;					//房间窗口
	TCHAR								m_szGameRoomName[61];			//房间名字

	//UserInfoStruct						*m_CurpUserInfoClicked;		//当前点击的用户指针
	//游戏变量
protected:
	bool								m_bWatchMode;					//旁观模式
	bool								m_bWatchOther;					//可以旁观
	BYTE								m_bWaitTime;					//等待时间
	CPtrArray							m_WatchPtrArray;				//旁观指针数组
	//========================================================================//
protected:
	std::list<MessageToSendStruct>		m_lstIPCMessage;				// 用来暂存IPC消息的队列
	bool								m_bIsGameValid;					// 游戏窗口是否起作用
};
