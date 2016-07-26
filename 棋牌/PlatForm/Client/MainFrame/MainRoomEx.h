#pragma once
#ifndef MAINROOMEX_HEAD_H
#define MAINROOMEX_HEAD_H

#include "apipe.h"//匿名管道 2012.08.20 yyf
#include "Socket32.h"


#include "Stdafx.h"
#include "GamePlace.h"
#include "BaseRoom.h"
#include "AutoUpdate.h"
#include "GameListCtrl.h"
#include "GameRoomEx.h"
#include "NavigationDlg.h"
#include "UserListCtrl.h"
#include "HttpDownloadDlg.h"

#include "AFCActWnd.h"					//大厅通知
#include "AfcTopWnd.h"					//顶部按钮
#include "DUITopWnd.h"
#include  "afcgamelistwnd.h"			//游戏例表窗口
#include "afCRoomRightWnd.h"
#include "Bulletin.h"

#include "PBankWndEx.h"
#include "BankVerifyWnd.h"
#include "MobileVerifyWnd.h"
#include "DownloadWnd.h"

//头像上传
#include "LogoUpload.h"
//IM
#include "IMMain.h"
//道具面板
#include "PropPannel.h"
#include "AppealDlg.h"

#include "GMListDlg.h"
#include "..\\UILibrary2003\\ShortMessage.h"
#include "gameplacemessage.h"
#include "GameListTip.h"

#include "CharmExchange.h"
#include "SafeCenterWnd.h"

#include "SkinMgr.h"
#include "PassDesk.h"
#include <vector>
#include <queue>
using namespace std;

//宏定义
#define MS_MAX						10									//最大任务数量
#define MAX_GAME_ROOM				3									//最大房间数目
#define WM_CLOSE_ROOM				WM_USER+216							//关闭游戏房间
#define WM_AUTO_GET_MONEY           WM_USER+500                         //自动赠送金币

//wushuqun 2008.10.14
#define WM_COLSE_ROOM_WITH_MSG      WM_USER+219                      //关闭房间后让大厅弹出一个消息框

#define IDM_SKIN					WM_USER+3
#define WM_REFLASNEWS				WM_USER+302							//房间读取新闻
#define WM_REFLAS_SYS_MSG			WM_USER+303							//房间读取系统消息

#define WM_GAME_LAUNCHED					WM_USER+439					///< 游戏启动过程结束
#define WM_GM_MSG							WM_USER+477
#define WM_GM_KICK							WM_USER+478

#define WM_GP_PROP							WM_USER+479					//游戏端道具消息

// PengJiLin, 2010-6-29, 混战场时间到, 关闭游戏。
#define WM_GAME_TIMEOUT                     WM_USER+442


#define WM_QUEUEROOM_EXIT                     WM_USER+443			//排队机房间解散时，已经同意的用户重新去排队，还没操作的用户点击什么都重新去排队
#define WM_USERCLIKE_AGREE                    WM_USER+444			//用户点击同意
#define WM_CLOSEGAME		                   WM_USER+445			//用户点击同意




//任务结构
struct MissionStruct
{
	bool							bAccess;							//是否可以访问
	UINT							uMissionID;							//任务 ID
	UINT							uKindID;							//类型 ID
	UINT							uNameID;							//名字 ID
};
//游戏房间信息
struct RoomInfoStruct
{
	//结构变量
	bool							bAccess;							//访问标志
	UINT							uTabIndex;							//房间索引
	DWORD							dwGameMSVer;						//主版本号
	DWORD							dwGameLSVer;						//副版本号
	CBaseRoom						* pGameRoomWnd;						//房间指针
	CRoomRightWnd					*pRoomInfoWnd;						//游戏左边
	TCHAR							szProcessName[31];					//进程名字
	ComRoomInfo						pComRoomInfo;						//设置数据
	CAFCRoomItem*					pRoomItem;					
};
//自动赠送提示信息 2013-01-31 wangzhitu
struct RECEIVEMONEYINFO
{
    CString strMsg;
	int iSec;
	RECEIVEMONEYINFO()
	{
		strMsg = "";
		iSec   = 0;
	}
};

//更新结构
typedef struct update
{
	int				uNameID;	//游戏ID
	CAFCRoomItem	*pGameRoomItem;	//
	HANDLE			hProcess;	//更新进程
}UPDATE,*LPUPDATE;

//add by wyl  11-5-16


//2012.08017 yyf
//匿名管道消息结构
typedef struct UpdateStructMsg
{  
	char command;//1: 父进程->子进程 ; 0:子进程->父进程
	int nRoomID;//房间id号 
	TCHAR g_GameName[200];//任务名 (游戏名
	TCHAR g_url[1000];//更新URL网址
	TCHAR g_ver[20];//版本号 
} sUpdateStructMsg;//每个更新任务，需要的信息，放在一个结构里.

class CMainRoomEx ;
typedef struct DownLoadUpdateMsg
{
	UpdateStructMsg *pUpdateStructMsg;
	UPDATE *pUPDATE;
	CMainRoomEx *pCMainRoomEx;
	CBcfFile *pCBcfFile;
} sDownLoadUpdateMsg;//传到更新下载线程里 2012.08.24 yyf

//游戏主大厅对话框
class CMainRoomEx : public CBaseRoom,public IAnonymousPipe,public ISimpleSocket
{
	CGameListTip *m_pListTip;
	static bool bTipInit;
	bool m_bListmessage;
	bool m_bInit;
	void OnConnectToCenterServerSucceed();
	bool m_bConnectingToCenterServer;
	bool m_bGetPara;
	//变量定义
public:
	static bool						m_bLockQuit;
	bool							m_bLogon;							//登陆成功
	CGameListWnd					*m_pGameListWnd;					//游戏列表
	CActWnd							*m_pActWnd;							//活动广告窗口
	CTopWndClass					*m_TopWnd;							//上方窗口
	CTopDuWndClass					*m_TopDuWnd;						//上方窗口Direct UI版
	CBulletin						*hBulletin;
	//add by wyl    11-5-13
	CAFCRoomItem					*m_pRoomItemInfo;					//游戏房间信息
	MSG_GP_R_SellGameListResult		m_SellListInfo;						//销售列表信息
	static char						m_szMobileVCode[36];				//从服务商发下来的手机验证码MD5密文

	BzDui::CButtonUI				*m_pUserHeadPic;					//玩家头像
	BzDui::CLabelUI					*m_pUserNickName;					//昵称
	BzDui::CLabelUI					*m_pUserGoldCoint;					//金币
	BzDui::CLabelUI					*m_pUserIdentity;					//用户ID
	BzDui::CLabelUI					*m_pUserBonus;						//奖牌

	CDownloadWnd					*m_pDownloadWnd;					//下载窗口

private:
	bool							m_bCheckVer;						//检测版本
	bool							m_bHideList;						//隐藏列表
	bool							m_bHideNews;						//隐藏公告
	CAutoUpdate						m_Update;							//自动更新
	MissionStruct					m_Mission[MS_MAX];					//任务数组
	vector<LPUPDATE>					m_UpdateList;						//正在更新列表
	CRITICAL_SECTION				m_Section;							//用于列表更新的同步

	//CAnonymousPipe m_AnonymousPipe;//匿名管道 2012.08.17 yyf
	InstanceLockBase m_InstanceLockBase;//同步锁对象 2012.08.22 yyf
	CSocket32      m_CSocket32;//本地进程间 网络操作  2012.08.24 yyf

	static UINT							m_onlineUserCount;					//在线人数(用于游戏大厅统计总在线人数)
public:
	RoomInfoStruct					m_RoomInfo[MAX_GAME_ROOM];			//房间信息
	MSG_GP_R_LogonResult			m_PlaceUserInfo;					//用户资料

	//界面图
private:
	CGameImage						m_mainlc,m_mainrc,m_maintc,m_mainbc;	//四周框
	CGameImage						m_mainlt,m_mainrt,m_mainlb,m_mainrb;//四角


	int								m_skx,m_sky;
	int								m_adx,m_ady,m_adheight,m_picx,m_picy,m_infox,m_infoy;
//	int								m_minx,m_miny,m_clx,m_cly,m_bx,m_by;
	int								m_wb;	//按钮之间的间隔
	CString							m_utitle;
	COLORREF						m_titlecolor;
	BOOL							m_bLockMathine;						//表示锁定机器功能是否启动 zxj 2009-11-12
	BOOL							m_bCanClick;						//表示当前锁定机器按钮是否可用
public:
	BOOL							m_HaveSellList;						//是否有销售列表

	//控件变量
public:

	CIEClass							m_WebView;							//网页视图

	CNavigationDlg						m_NavigationDlg;					//导航窗口

	CNormalBitmapButtonEx					m_Bt1;						//
	CNormalBitmapButtonEx					m_Bt2;						//
	CNormalBitmapButtonEx					m_Bt3;						//
	CNormalBitmapButtonEx					m_Bt4;						//
	CNormalBitmapButtonEx					m_Bt5;						//
	CNormalBitmapButtonEx					m_Bt6;
	CNormalBitmapButtonEx					m_Bt7;
	CNormalBitmapButtonEx					m_Bt8;
	CNormalBitmapButtonEx					m_Bt9;
	CNormalBitmapButtonEx					m_Bt10;

	int										m_mainbtnX;				//上方按钮位置起始坐标
	int										m_mainbtnSpace;			//上方按钮之间空隙
	bool									m_bShowSysmsg;			//是否下载最新系统消息
	CLogoUpload								*logoUpload;			//头像上传对象
	CPropPannel								*propPannel;			//道具面板对象

	static bool								m_bBankLogoned;			//已登陆银行?

	CCharmExchange							*CharmPannel;

	//投诉对话框
	CAppealDlg                              *m_pAppealDlg;            //投诉对话框

	//投诉对话框
	CGMListDlg                              *m_pGMListDlg;            //投诉对话框

	CSafeCenterWnd							*m_pSafeCenterWnd;		  //安全中心对话框

	CPBankWnd								* m_pBankWnd;			  //旧版个人银行
	CBankVerifyWnd							* m_pBankVerifyWnd;		  //新版个人银行，登陆框
	CPBankWndEx								* m_pBankWndEx;			  //新版个人银行

	CMobileVerifyWnd						* m_pMobileVerifyWnd;	  //手机验证窗口


	map<int, CShortMessage*>				m_pGMMsgDlgs;

	MSG_MarQueueInfo						m_tMarQueueInform;		//走马灯信息

#ifdef AI_MACHINE
	int										m_Airoom;
#endif

	void checkNetSignal(void);
    void getNetSignal(void * pNetData);
    void drawSignal(void);
    int m_NetSignalIndex;                //本次网络信息检测序号
    int m_SavedSingalIndex;     //上一次发送的网络信息检测序号，收到消息后与m_NetSingalIndex同步
    int m_NetSignalLevel;                //网速信号级别
    Image  *img;

    // PengJiLin, 2010-9-13, 踢人卡、防踢卡使用时间
    int     m_iKickPropTotalTime;       // 踢人卡到期时间
    int     m_iAntiKickPropTotalTime;   // 防踢卡到期时间

	int		m_nKeepAliveCount;			//心跳	
#ifdef MY_QUICK_GAME_ON

	// JianGK 2011/09/20,命名管道,用于外部通讯,控制平台开发一些便于测试的功能
	HANDLE m_hQGNamedPipe; //外部通讯的管道
	HANDLE m_hQGThread;  //m_hNamedPipe通讯处理线程句柄
	UINT m_uQGRoomID; //要快速启动的房间ID

#endif
	

	struct TQGInfo 
	{
		UINT uNameID;
		UINT uRoomID;

		TQGInfo()
		{
			uNameID = 0;
			uRoomID = 0;
		}
	};


	//函数定义
public:
	//构造函数
	CMainRoomEx();
	//带例表和走马灯
	CMainRoomEx(CGameListWnd *pGameListWnd, CActWnd *pActWnd, CTopWndClass *pTopWnd,CTopDuWndClass *pTopDuWnd);
	//析构函数
	virtual ~CMainRoomEx();

	DECLARE_MESSAGE_MAP()

	//重载函数
public:
	//数据绑定函数
	virtual void DoDataExchange(CDataExchange * pDX);
	//初始化函数
	virtual BOOL OnInitDialog();
	//消息解释
	virtual BOOL PreTranslateMessage(MSG * pMsg);

	//接口函数
public:
	//询问关闭
	virtual bool CanCloseNow() { return true; };
	//关闭窗口
	virtual bool CloseGameRoom() { return true; };
	//位置变化u
	virtual bool OnSizeEvent(UINT nSizeType, int cx, int cy);
	//拆分条消息
	virtual bool OnSplitEvent(UINT uSplitID, UINT uXPos, UINT uYPos);
	//调整位置
	virtual void FixControlStation(int iWidth, int iHeight);
	//网络读取消息
	virtual bool OnSocketReadEvent(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket);
	//网络连接消息
	virtual bool OnSocketConnectEvent(UINT uErrorCode, CTCPClientSocket * pClientSocket);
	//网络关闭消息
	virtual bool OnSocketCloseEvent();
	//取消连接消息
	virtual bool OnCancelConnectEvent();
	//菜单命令
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	
	//功能按钮
	void OnBnClickedBt1();////
	
	void OnBnClickedBt2();/////////
	
	void OnBnClickedBt3();/////////
	
	void OnBnClickedBt4();/////////
	
	void OnBnClickedBt5();/////////

	void OnBnClickedBt6();////
	
	void OnBnClickedBt7();/////////
	
	void OnBnClickedBt8();/////////
	
	void OnBnClickedBt9();/////////
	
	void OnBnClickedBt10();/////////

	//判断游戏是否需要更新
	bool IsGameNeedUpdate(ComNameInfo *pNameInfo);
	bool IsGameNeedUpdate(ComRoomInfo *pRoomInfo, ComNameInfo* &pNameInfo);

	//下载游戏
	void DownloadGame(ComNameInfo* pNameInfo);
	//下载游戏(通过房间获取游戏类型)
	void DownloadGameEx(ComRoomInfo *pRoomInfo);

	//辅助函数
private:
#ifdef AI_MACHINE
	bool EnterRoom(int roomid);
#endif
public:
	DECLARE_EVENTSINK_MAP()
	//下载新闻
public:
	static DWORD WINAPI DownLoadNewIni(LPVOID lparam);
	//读写新闻
	void WriteNews();
	
	void IE(LPCTSTR URL);
	//版本检测
	bool GamePlaceVerCheck(MSG_S_ConnectSuccess * pVerInfo, bool & bNewVer);
	//增加任务
	bool AddMission(UINT uMissionType, UINT uKindID, UINT uNameID);
	//删除任务
	bool DeleteMission();
	//处理任务
	bool HandleMission();
	//发送登陆信息
	bool SendUserInfo();
	//发送头像更改消息
	bool SendSetLogoInfo(long int iUserID, UINT iLogoID);
	//获取游戏列表
	bool GetGameRoomList();
	//获取最近登录的游戏
	bool GetLastLoginGame();
	///获取兑换列表
	bool GetCharmExchangeList();
	//连接消息处理
	bool OnConnectMessage(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket);
	//登陆 消息处理
	bool OnLogonMessage(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket);
	//银行 消息处理
	bool OnBankMessage(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket);
	//列表信息
	bool OnListMessage(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket);
	//系统消息
	bool OnSystemMessage(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket);
	//进入游戏房间
	bool OnCentranceGameRoom(CAFCRoomItem * pGameRoomItem);
	//发送数据到服务端，请求比赛房间号
	bool OnGetRoomInfo(CAFCRoomItem * pGameRoomItem);
	//网页URL变化监测，JianGuankun 2012.06.26
	void BeforeNavigate2Explorer(LPDISPATCH pDisp, VARIANT* URL, VARIANT* Flags, VARIANT* TargetFrameName, VARIANT* PostData, VARIANT* Headers, BOOL* Cancel);
	void NavigateErrorExplorer(LPDISPATCH pDisp, VARIANT* URL, VARIANT* Frame, VARIANT* StatusCode, BOOL* Cancel);

	//处理形象变化，JianGuankun 2012.06.26
	void ProcessMyAvatarChange(UINT uNewLogoID);

	///提炼出来的进入房间函数 lxl 2010-10-08
	bool EnterRoom(CAFCRoomItem * pGameRoomItem);
	//锁定机器结果解析函数 zxj 2009-11-16
	void OnLockAccountResult( NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket);

	void ShowCharmPannel();

	bool GetInGame();

    void CloseSafeCenterWnd();
	//获取在线人数
	UINT GetonlineUserCount();
private:
	//设置在线人数
	void SetonlineUserCount(UINT onlineUserNum);
	//功能函数
public:
	//关闭所有房间
	bool CloseAllGameRoom();
	//绘画框架
	void DrawViewFrame(CDC * pDC, int iWidth, int iHeight);

    // PengJiLin, 2010-9-13, 踢人卡、防踢卡功能，设置使用时间
    void SetKickPropTime(int iKickTime);
    void SetAntiKickPropTime(int iAntiKickTime);

	string JoinUrl(string p1, string p2);
	void char2Tchar(char *pchar,TCHAR * tchar);

    // PengJiLin, 2011-6-23, 激活消息
    void SetActiveToMainRoom(UINT uState);

	RoomInfoStruct* GetCurrentGameRoom();


#ifdef MY_QUICK_GAME_ON

	//JianGK,创建通讯管道
	bool OpenQuickGameNamedPipe(char* szPipeName);

	//JianGK,关闭通讯管道
	bool CloseNamedPipe();

	//JianGK,管道通讯线程
	//static DWORD WINAPI NamedPipeThread(LPVOID param);
	static UINT AFX_CDECL NamedPipeThread(LPVOID param);

#endif // MY_QUICK_GAME_ON

public:
	CPassDesk pdDlg;
	//消息函数
public:
	//初始化
	void Init(const CString& strLogonUserID = _T(""));
	//网页按钮
	void OnWeb();
	//修改资料
	//void OnChangeInfo();
	//进入游戏房间
	void OnInGame();
	//退出游戏
	void OnQuitPlace();
	//关闭房间请求
	LRESULT OnCloseGameRoom(WPARAM wparam, LPARAM lparam);
	//自动赠送金币2013-01-31 wangzhitu
	LRESULT onAutoGetMoney(WPARAM wparam, LPARAM lparam);

	//wushuqun 2008.10.14
	LRESULT OnCloseGameRoomWithMsg(WPARAM wparam, LPARAM lparam);
	
	
	LRESULT OnLoadSkin(WPARAM wparam, LPARAM lparam);
	LRESULT OnExchangeSkin(WPARAM wparam, LPARAM lparam);
//隐藏公告新闻按钮
	//void OnBnClickedHideNews();  //ON_COMMAND(IDM_CONNECT_CENTER_SERVER, OnConnectToCenterServer)
	//隐藏列表按钮
	//void OnBnClickedHideList();  //ON_COMMAND(IDM_CONNECT_CENTER_SERVER, OnConnectToCenterServer)

	afx_msg void OnConnectToCenterServer();
	//登录游戏大厅
	afx_msg void OnLogonPlace();
	//右击游戏列表
	//afx_msg void OnNMRclickGameList(NMHDR * pNMHDR, LRESULT * pResult);
	//双击游戏列表
	//afx_msg void OnNMDblclkGameList(NMHDR * pNMHDR, LRESULT * pResult);
	//游戏列表项改变
	//afx_msg void OnTvnSelchangedGameList(NMHDR * pNMHDR, LRESULT * pResult);
	//列表正在展开
	//afx_msg void OnTvnItemexpandingGameList(NMHDR * pNMHDR, LRESULT * pResult);
	//定时器消息
	afx_msg void OnTimer(UINT nIDEvent);
	//重画函数
	afx_msg void OnPaint();
	//显示窗口
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	//最小化按钮
	//void OnBnClickedMin();
	//关闭按钮
	//void OnBnClickedClose();
	//afx_msg void OnBnClickedSkin();

//	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
public:
	//afx_msg void OnBnClickedShowmenu();
public:
	//afx_msg void OnBnClickedBt6();
public:
	//afx_msg void OnBnClickedBt7();
public:
	void LoadSkin(const CString& strLogonUserID = _T(""));
	//显示还是隐藏左边
	LRESULT OnShowLeft(WPARAM ,LPARAM);

	LRESULT OnTopTabSelected(WPARAM wparam,LPARAM lparam);
	LRESULT OnTopTabLostSelected(WPARAM wparam,LPARAM lparam);

	//打开IE
	LRESULT OnOpenIE(WPARAM ,LPARAM);

	LRESULT OnCallGM(WPARAM wParam, LPARAM lParam);

	LRESULT OnOpenBank(WPARAM wParam, LPARAM lParam);

	//投诉  yjj 090310
	LRESULT OnAppeal(WPARAM ,LPARAM);
	LRESULT OnLockAccount(WPARAM wParam, LPARAM lParam);	//锁定机器 zxj 2009-11-12

	
	//上传头像
	LRESULT OnUpLoadUserLogo(WPARAM,LPARAM);
//检测玩家头像
void checkUserLogo(long dwUserID);
public:
	bool StartDownLoad(TCHAR filename[],CAFCRoomItem * pGameRoomItem);

	bool CheckUpdate(TCHAR filename[],CAFCRoomItem * pGameRoomItem);
public:
	//获得房间信息结构指针
	bool GetRoomInfoStruct(RoomInfoStruct** pRoomInfoStruct);
	//更新本人信息
	BOOL UpdateMeInfo(__int64 i64Money,BOOL IsUpdateMoney,int dwFascination,BOOL IsUpDateFascination);
	//

	//更新所有GameFrame的银行金币数信息
	//用于玩家打开多个游戏银行，其中一个银行有变化，把变化同步到其它房间或银行窗口
	//JianGuankun 2012.1.5
	void UpdateBankMoneyToAllRoom(__int64 i64Money,CBaseRoom* pLaunchRoom = NULL);

	void ChangeImageList(COLORREF color);
public:
	void OpenBulletin(void);
public:
	void propGetListRequest(void);
public:
	//获取机器码相关函数 zxj 2009-11-12 锁定机器
	static CString innerGetMac();			//取得网卡硬件地址
	static CString coreGetCode();	//取得机器码

public:
	void	OnSwitchLockButton();	//切换锁定机器按钮状态

	void OnSendMobileVCodeSms(char* szMobileNo,UINT uType);			//发送手机验证码短信 JianGuankun 2012.5.16
	static bool	ValidMobileVCode(char* szVCode);					//验证用户输入的手机验证码是否正确 JianGuankun 2012.5.16
	static void ClearMobileVCode();									//擦除手机验证码，使验证码过期 JianGuankun 2012.5.16
	static CString GetUrlValue(CString szUrl, CString szField);		//获取URL参数

	//匿名管道回调 读 接口 2012.08.20 yyf
	virtual void ReadAnonymousPipe(LPVOID lpBuffer, DWORD dwNumberOfBytesToRead);//2012.08.17 yyf
	virtual  void ReadSimpleSocket(LPVOID lpBuffer, DWORD dwNumberOfBytesToRead);//本地进程间网络 回调接口 2012.08.24 yyf
	static DWORD WINAPI DownLoadUpdate(LPVOID lparam);//下载更新线程  2012.08.24 yyf

private:
	CSkinMgr m_skinmgr;
};

//获取游戏大厅指针
extern CMainRoomEx * GetMainRoom(void);

#endif