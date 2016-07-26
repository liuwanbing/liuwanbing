// Auto_AIDlg.h : 头文件
//
#include "stdafx.h"
#include "resource.h"

#include "CTime_day.h"


#include <vector>
#include <functional>
#include <algorithm>
#include "afxwin.h"
using namespace std;

#pragma once

#define				 MAX_GAME_ROOM				2000			//最大房间数
#define				 MAX_LOGON_COUNT			10
#define				 TIME_NEXT_ID				1				//下一个帐号


#define				 TIME_LOGIN				2				//登录事件
#define				 TIME_CHECKLOGIN		3				//定时登录
#define				 TIME_CHECKRELOGIN		4				//定时登录
#define				 TIME_CHECKRESOCKETERROR		5				//定时登录

#define					TIME_CHECKBCONNECT_TIMEOUT	6		//檢測B服務器連接超時
#define					TIME_CHECKZCONNECT_TIMEOUT	7		//檢測Z服務器連接超時

#define              TIME_AUTO_LOGIN            100               // 一定时间内自动登入防断线
#define				 TIME_SPACE					1000
#define				 IDM_CLOSE_GAME_ROOM		WM_USER+200		//关闭房间
#define				 IDM_LOGON_FAIL				WM_USER+201		//登陆失败
#define				 IDM_UPDATE_USER			WM_USER+202		//更新玩家显示

//#define DDZ
//游戏房间信息
struct RoomInfoStruct
{
	//结构变量
	bool							bAccess;							//访问标志
	UINT							uTabIndex;							//房间索引
	DWORD							dwGameMSVer;						//主版本号
	DWORD							dwGameLSVer;						//副版本号
	CBaseRoom						* pGameRoomWnd;						//房间指针
	TCHAR							szProcessName[31];					//进程名字
	ComRoomInfo						stComRoomInfo;						//设置数据
	void operator=(RoomInfoStruct & paiData)
	{
		bAccess = paiData.bAccess;
		uTabIndex = paiData.uTabIndex;
        dwGameMSVer = paiData.dwGameMSVer;
		dwGameLSVer = paiData.dwGameLSVer;
		pGameRoomWnd = paiData.pGameRoomWnd;
		memcpy(&stComRoomInfo,&paiData.stComRoomInfo,sizeof(stComRoomInfo));
		memcpy(&szProcessName,&paiData.szProcessName,sizeof(szProcessName));
	}
};

//游戏房间信息


struct TTimeRange;

struct TRobotItem
{
	int iIndex;
	int iUserID;
	RoomInfoStruct* pRoomItem;
	
	TTimeRange* pTimeRanges;
public:
	TRobotItem()
	{
		ZeroMemory(this, sizeof(TRobotItem));
	}
};


struct TTimeRange
{
	TTime_day	tBegin, tEnd;
	int		iCount;
	bool	iLogoned;
	bool GetValid()
	{
		CTime t = CTime::GetCurrentTime();
		TTime_day t_day(t.GetHour(), t.GetMinute());

		int r1 = CmpTime_dat(tBegin, t_day);
		int r2 = CmpTime_dat(t_day, tEnd);
		return (r1 <= 0 &&  r2 < 0);
	}
	vector<TRobotItem*>	m_Robots;
public:
	TTimeRange(TTime_day tbegin, TTime_day tend, int icount)
	{
		tBegin = tbegin;
		tEnd = tend;
		iCount = icount;
		m_Robots.clear();
		iLogoned = false;
	}
};


struct TTime_Envet: public TTime_day
{
	int iFlag;
	int iCount;
	CString GetStr()
	{
		CString r, tmp;
		if (iFlag == 0)
			tmp = "登录";
		else
			tmp = "退出登录";
		r.Format("%s%s人数【%d】", getTimeStr(), tmp, iCount);
		return r;
	}
};


//



// CAuto_AIDlg 对话框
class CAuto_AIDlg : public CBaseRoom
{
	// 构造
public:
	//构造函数
	CAuto_AIDlg(CWnd * pParent=NULL);
	~CAuto_AIDlg();

	// 对话框数据
	enum { IDD = IDD_AUTO_AI_DIALOG };
	int		m_nInRoomCount;
	static int                      m_nRoomPeopleSize;                  //定义房间人数，房间人数大于这个值时机器人自动离开房间

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


	// 实现
protected:
	HICON									m_hIcon;
	BYTE									m_bConnectToWitchServer;
	CString									m_CenterServerIPAddr;
	int										m_CenterServerPort;
	int										m_iEnterKindID;								//游戏类型ID							
	int										m_iEnterNameID;								//游戏名称ID
	int										m_iEnterRoomID;								//登陆房间ID
	CGameListCtrl							m_GameList;									//游戏列表
	MSG_GP_R_LogonResult					m_PlaceUserInfo;							//用户资料
	RoomInfoStruct							m_RoomInfo[MAX_GAME_ROOM];					//房间信息

	TRobotItem								m_Robots[MAX_GAME_ROOM];			
	vector<TTimeRange>						m_TimeRanges;


	int							        	m_bNowRoomIndex;							//当前房间索引
	int										m_iAccountsStart;							//帐号起始
	int                                     m_iAccountsStartTemp;                       //帐号起始，临时记录
	int										m_iCurrentAccounts;							//当前帐号
	int										m_bCountLogon;								//准备登陆多少号
	TCHAR									m_CurrentLogonName[21];						//当前登陆昵称
	TCHAR									m_CurrentNickName[21];						//当前登陆昵称
	BYTE									m_bEnterStyle;								//登陆属性
	TCHAR									m_szNickFix[21];							//昵称前缀
	TCHAR									m_szNameFix[21];							//注册名前缀
	bool									m_bAutoRegister;							//注册
	bool                                    m_bMachineAndPlay;							//机器人与玩家是否同桌
	bool                                    m_bAutoDesk;								//机器人自动换桌
	
	int                                     m_iMachineCheckTime;//机器人检测间隔
	BYTE                                    m_bMachineDeskCount;//每桌机器人个数
	int										m_iRealLogonCount;							//实际成功登陆人数
	__int64									m_i64MaxAiMoney;                    //AI身上最多的钱数，达到这个数就存钱
	__int64									m_i64CheckMoneyMax;                 //取钱最大数
	__int64									m_i64CheckMoneyMix;                 //取钱最小数
	__int64									m_i64SaveAndOutMoneyBase;			//存取钱基数（整存整取基数）
	BOOL                            m_bAiBank;                          //是否机器人操作银行
	BOOL							m_bOpenBankBase;							//是否开启机器人整存整取
	int                             m_nAiLogonTime;                     //机器人进入房间间隔

	CGameRoomEx * m_pGameRoom  ;
	// 生成的消息映射函数
	virtual BOOL OnInitDialog();

	afx_msg void OnPaint();

	afx_msg HCURSOR OnQueryDragIcon();

	DECLARE_MESSAGE_MAP()
	//确定函数
	virtual void OnOK() ;

	void OnAutoOK();
	//取消函数
	virtual void OnCancel();
	//显示提示
	void ShowCue(TCHAR *pMessage);
	//
	void ShowCue(CString pMessage);
	//连接 
	void OnConnectToCenterServer();
	//登陆服务器
	void OnConnectToLogonServer();
	//连接消息处理
	bool OnConnectMessage(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket);
	//登陆 消息处理
	bool OnLogonMessage(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket);
	//列表信息
	bool OnListMessage(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket);
	//处理服务端发送的比赛房间ID
	bool EnterContestRoom(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket);
	//进入游戏房间
	bool OnCentranceGameRoom(CAFCRoomItem * pGameRoomItem);
	//关闭房间请求
	LRESULT OnCloseGameRoom(WPARAM wparam, LPARAM lparam);
	//直接登陆
	BOOL SendUserInfo();////发送用户信息
	//某个帐号登陆失败 20081128 by Shao
	LRESULT OnLogonFail(WPARAM wparam, LPARAM lparam);
	LRESULT OnUpdateMoney(WPARAM wparam, LPARAM lparam);

	///玩家是否在线，在线就不重登
	BOOL PlayerOnLine(TCHAR szCurrentName[]);

	//20091123
	CString coreGetCode();
	CString innerGetMac();
	//获取游戏列表
	bool GetGameRoomList();
	//进入房间
	bool EnterRoom(int roomid);
	//计时器
	void OnTimer(UINT_PTR nIDEvent);
	//重新连接
	bool ReConnectToCenterServer();
	//接口函数
public:
	//网络读取消息
	virtual bool OnSocketReadEvent(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket);
	//网络连接消息
	virtual bool OnSocketConnectEvent(UINT uErrorCode, CTCPClientSocket * pClientSocket);
	//网络关闭消息
	virtual bool OnSocketCloseEvent();
protected:
	//发送网络消息函数
	inline virtual int SendGameData(BYTE bMainID, BYTE bAssistantID, BYTE bHandleCode);
	//发送网络消息函数
	inline virtual int SendGameData(void * pData, UINT uBufLen, BYTE bMainID, BYTE bAssistantID, BYTE bHandleCode);
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//核对
	BOOL CheckVersion();
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnBnClickedClose();
public:
	afx_msg void OnBnClickedMix();
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
public:
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);

	

public:
	afx_msg void OnBnClickedButton3();
public:
	afx_msg void OnBnClickedOk();



public:
	bool m_bUseNew;
	bool m_bLogoning;
	int m_iCurLogonIndex;
	int	m_iSumLogonCount;
	bool m_bExistLogonTimer;
	void LogonRobot_New();
	void InitRobot_New();
	void OnTimeLogon_New();
	void OnLogon_New();
	void OnGameList_New();
	void SendUserInfo_New();
	void ConnectToB_New();
	void ConnectToZ_New();
	void ConnectToW_New(int roomid);
	void GetContestRoomID(int roomid);//向服务器获取比赛房间ID
	void StartRun_New();
	void ReLogonRobot_New();
	void CheckLogonRobot_New();
	void CheckSocketError_New();
	int	FindNextLogonIndex(int iIndex);
	CListBox m_EventList;
	vector<TTime_Envet> m_EventTimes;
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedCheck6();
	afx_msg void OnBnClickedCheck7();
};
