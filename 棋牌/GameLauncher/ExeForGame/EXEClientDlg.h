// EXEClientDlg.h : 头文件
//

#pragma once

#include "stdafx.h"
#include "LoveSendClassInExe.h"
// IPC相关
#include "IPC\common.h"
#include "IPC\sink.h"
#include "IPC\ChannelService.h"
#include "SkinMgr.h"

#define TIMER_HEART_BEAT		100	// 用来维持IPC心跳的定时器
const int HEART_BEAT_SECOND		= 3;	// 心跳间隔
const int HEART_BEAT_DELAY		= 10;	// 检测心跳延时

#define TIMER_DRAW              101
#define FRAME_INTERVAL          20

class IGameEngine;
// CEXEClientDlg 对话框
class CEXEClientDlg : public CDialog, public IChannelMessageSink
{
// 构造
public:
	CEXEClientDlg(CWnd* pParent = NULL);	// 标准构造函数
	~CEXEClientDlg();

// 对话框数据
	enum { IDD = IDD_MAIN_DLG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


	//========================================================================\\
									IPC支持
public:
	bool SetupIPC();
	//信道数据处理
	virtual bool __cdecl OnChannelMessage(const IPC_Head * pHead, const void * pIPCBuffer, WORD wDataSize, HWND hWndSend);
	//控制通知
	bool OnIPCControl(const IPC_Head * pHead, const void * pIPCBuffer, WORD wDataSize, HWND hWndSend);
	//网络指令
	bool OnIPCSocket(const IPC_Head * pHead, const void * pIPCBuffer, WORD wDataSize, HWND hWndSend);
	//设置指令
	bool OnIPCConfig(const IPC_Head * pHead, const void * pIPCBuffer, WORD wDataSize, HWND hWndSend);
	//用户动作
	bool OnIPCUserAct(const IPC_Head * pHead, const void * pIPCBuffer, WORD wDataSize, HWND hWndSend);
	//道具初始化
	bool OnIPCProp(const IPC_Head * pHead, const void * pIPCBuffer, WORD wDataSize, HWND hWndSend);
	//发送数据
	bool SendSocketData(WORD wMainCmdID, WORD wSubCmdID, void * pData, WORD wDataSize);
	//硬件加速
	void SetAcceleration(int iLevel);
public:
	//保存玩家原来的硬件加速设置
	int		m_iLevel;
	int     m_hwAcc;
private:
	HANDLE								m_hShareMemory;
	tagShareMemory						* m_pShareMemory;				//共享内存
	CChannelService						*m_ChannelService;
	DWORD								m_dwIPCHeartBeatTick;			// 用来记录心跳消息接收时间的Tick数
	//========================================================================//


// 实现
protected:
	HICON m_hIcon;
	HINSTANCE						m_hGameInstance;					//游戏进程
	//bool							m_bNeedRefleshProp;					//需要刷新道具
	//int								m_nPropCount;						//从平台传来的道具数量
	CArray<_TAG_USERPROP*,_TAG_USERPROP*> m_userPropLibrary;							//用户拥有的道具


	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
public:
	afx_msg void OnBnClickedCancel();

private:
	GameInfoStructInExe		m_GameInfo;
	IFrameInterface			* m_IGameFrame;						//游戏框架
    IGameEngine             * m_pNewGame;                       //新游戏
//	CLoveSendClassInExe		*m_pLoveSendClassInExe;
	LRESULT OnGameQuit(WPARAM wParam, LPARAM lParam);
	LRESULT OnRequestPlayList(WPARAM wParam, LPARAM lParam);
	LRESULT OnSendGameData(WPARAM wParam, LPARAM lParam);
	LRESULT OnSetStation(WPARAM wParam, LPARAM lParam);
	LRESULT OnSendBroadData(WPARAM wParam, LPARAM lParam);
	LRESULT OnSendBuyProp(WPARAM wParam, LPARAM lParam);
    LRESULT OnSendBuyPropNew(WPARAM wParam, LPARAM lParam); // PengJiLin, 2010-10-13, 商店道具的即时购买功能
	LRESULT OnSendGiveProp(WPARAM wParam, LPARAM lParam);
	LRESULT OnSendBuyVIP(WPARAM wParam, LPARAM lParam);      //道具面板传回的购买VIP消息， add by wyl   11-5-21

	LRESULT OnSendAddFriend(WPARAM wParam, LPARAM lParam);      //添加好友
	LRESULT OnSendAddFriend_R(WPARAM wParam, LPARAM lParam);      //添加好友
	LRESULT OnSendGetFriendList(WPARAM wParam, LPARAM lParam);      //添加好友

	// 黑名单消息
	LRESULT OnSendAddEmeny(WPARAM wParam, LPARAM lParam);      // 加入黑名单
	LRESULT OnSendAddEmeny_R(WPARAM wParam, LPARAM lParam);    // 加入黑名单
	// 

    // 激活消息
    LRESULT OnActiveToRoom(WPARAM wParam, LPARAM lParam);
	// 游戏窗口移消息
	LRESULT OnGameWinMove(WPARAM wParam, LPARAM lParam);

	//游戏改变分辨率消息
	LRESULT OnScreenSizeChange(WPARAM wParam, LPARAM lParam);

	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);

private:
	CSkinMgr m_skinmgr;
};
