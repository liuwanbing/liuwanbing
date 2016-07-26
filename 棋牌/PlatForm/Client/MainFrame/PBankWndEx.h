#pragma once

#ifndef PBANKENDEX_HEAD_H
#define PBANKENDEX_HEAD_H

#include "StdAfx.h"
#include <list>
#include <vector>
using namespace std;

/*------------------------------------------------------------------------------------
	名词约定：
	1.窗口左则Tab表的大按钮称为"类型按钮";
	2.点击类型按钮会列出若干item,每个item称为"功能项"；
	3.每个类型按钮所对应若干item集合称为"该类型的功能列表";
	4.每个功能项原则上在窗口右方都配有一个容器，放置使用功能项所需要的控件,
	  称为该功能的"封装容器"。

  ----------------------------------------------------------------------------------*/

const int WM_BANK = WM_USER + 600;

// JianGuankun,2012-1-4,根据用户ID取昵称，或根据昵称取用户ID
const int WM_BANK_GET_NICKNAMEID = WM_USER + 601;

// JianGuankun, 2012-1-4, 根据ID获取昵称,或昵称获取ID
struct GetNickNameID_t
{
	long int _user_id;       //用户ID
	TCHAR	 _nickname[61];  //用户昵称
	BYTE	 _nType;	     //0代表通过用户ID取用户昵称,1代表通过用户昵称取用户ID
	BYTE	 _nUseful;		 //用途标识				
	GetNickNameID_t()
	{
		ZeroMemory(this, sizeof(GetNickNameID_t));
	}
};


class CPBankWndEx;

typedef void (CPBankWndEx::*lpNotifyFun)(BzDui::TNotifyUI& msg);	//功能项UI操作容器消息响应函数

class CPBankWndEx : 
	public BzDui::CWindowWnd,
	public BzDui::INotifyUI,
	public BzDui::IMessageFilterUI,
	public BzDui::IListCallbackUI
{

public:

	CPBankWndEx(CWnd* pParent,bool bInMainRoom = false);
	virtual ~CPBankWndEx(void);

	LPCTSTR GetWindowClassName() const;

	UINT GetClassStyle() const;

	void OnFinalMessage(HWND hWnd);

	void Init();

	//关于取出金币
	void Init_Ctn1_1();

	//关于存入金币
	void Init_Ctn1_2();

	//关于转账
	void Init_Ctn1_3();

	//关于存单办理
	void Init_Ctn1_5();

	//关于充值
	void Init_Ctn1_7();

	//关于修改密码
	void Init_Ctn2_1();

	//关于安全设置
	void Init_Ctn3_1();

	void Notify(BzDui::TNotifyUI& msg);

	void CenterWindow();

public:

	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT OnNcActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

	LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);

	virtual LPCTSTR GetItemText(BzDui::CControlUI* pControl, int iIndex, int iSubItem);

public:

	bool OnNetMessage(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket);

	//“类型1”中的“标题1”消息处理函数
	void Notify_1_1(BzDui::TNotifyUI& msg);

	//“类型1”中的“标题2”消息处理函数
	void Notify_1_2(BzDui::TNotifyUI& msg);

	//“类型1”中的“标题3”消息处理函数
	void Notify_1_3(BzDui::TNotifyUI& msg);

	//“类型1”中的“标题4”消息处理函数
	void Notify_1_4(BzDui::TNotifyUI& msg);

	//“类型1”中的“标题5”消息处理函数
	void Notify_1_5(BzDui::TNotifyUI& msg);

	//“类型1”中的“标题6”消息处理函数
	void Notify_1_6(BzDui::TNotifyUI& msg);

	//“类型1”中的“标题7”消息处理函数
	void Notify_1_7(BzDui::TNotifyUI& msg);

	//“类型2”中的“标题1”消息处理函数
	void Notify_2_1(BzDui::TNotifyUI& msg);

	//“类型3”中的“标题1”消息处理函数
	void Notify_3_1(BzDui::TNotifyUI& msg);

public:	///< 外部调用的接口
	
	///
	/// 设置财富信息
	/// @param nBank 银行存款
	/// @param nWallet 手中现金
	/// @return void
	///
	void SetWealthInfor(__int64 i64Bank, __int64 i64Wallet);

	///
	/// 更新财富信息
	/// @return void
	///
	void UpdateWealthInfor();

	///
	/// 提示
	/// @param pToCtrl 在某一控件上提示
	/// @param lpSecName ClientMessage.bcf中的段名
	/// @param lpKeyName ClientMessage.bcf中的键值
	/// @param lpDefault ClientMessage.bcf没有找到键值时，使用的默认值
	/// @return void
	///
	static void ShowTipMessage(BzDui::CControlUI* pToCtrl,LPCTSTR lpSecName,LPCTSTR lpKeyName,LPCTSTR lpDefault,...);

	///根据用户ID获取昵称
	bool OnIDKillFocus(BzDui::CEditUI* pEdtCtrl,BYTE nType,BYTE nUserful);

	/// 转帐成功后更新银行数据
	/// @param Opt 0-表示转出给别人，1-表示由别人转入给自己
	/// @param nMoney 表示本次所操作的金币数
	void UpdateUserMoneyByTransfer(int OptType,__int64 i64Money);

	///
	/// 收到一条转账记录
	/// @param void
	/// @return void
	///
	void OnReceivedTransferRecord(MSG_GR_R_TransferRecord_t *pRecord);

	///刷新记录的页码
	void UpdateRecordPageState();

	///更新用户ID昵称的查询结果
	void UpdateNicknameIDResult(GetNickNameID_t* pNID);

public:

	//加载常规控件皮肤
	void LoadNormalCtrlSkin();

	//为某一项功能UI容器绑定消息响应函数
	void BindNotify(UINT uSortIdx,BzDui::CStdString strCtnName,lpNotifyFun lpFunPtr,bool bNeedSubmit = true,BzDui::CStdString strBntName = "提交");

	//加载各功能UI容器的消息函数
	void LoadNotifyFunctions();

	//初始化游戏钱包数据
	void InitGameWalletTable();


private:

	//加载Combol的列表项
	void LoadComboItems(BzDui::CComboUI* pCtrl,LPCTSTR items[],int nums,int nSelect = 0);

	//根据文本设置选择列表项
	bool SelectComboItem(BzDui::CComboUI* pCtrl,LPCTSTR szText);

	//展开某类型子项功能列表
	void ShowFuns(int nIdx);

	//金额转中文大写
	void ChangeDigitToChinese(__int64 i64Num, CString &strResult);

	//把数字输入框，在输入过程中同步转换为中文大写
	void ChangeEtToChinese(BzDui::CEditUI* pEditCtrl,BzDui::CTextUI* pTextCtrl);

public:

	//通过游戏NameID,得到游戏的名字
	char* GetGameNameById(UINT uNameID);

	//通过游戏Name,得到游戏的NameID
	UINT GetGameIDByName(LPCTSTR pGameName);

	//通过游戏NameID,得到游戏的索引
	int GetWalletIndexById(UINT uNameID);

	//通过游戏NameID,设置其钱包的钱
	bool SetGameWalletMoney(UINT uNameID,__int64 NewMoney);

	//通过游戏NameID,设置其钱包添加钱
	bool AddGameWalletMoney(UINT uNameID,__int64 AddMoney);

	//通过游戏Name,获取钱包的钱
	__int64 GetGameWalletMoney(LPCTSTR pGameName);

	//通过游戏NameID,获取钱包的钱
	__int64 GetGameWalletMoney(UINT uNameID);

public:

	BzDui::CPaintManagerUI m_pm;

	//当前实例窗口句柄
	static HWND m_hInstanceWnd;

private:

	// 父窗口指针
	CWnd* m_pRoom;

	//父窗口的类型
	enum
	{
		IsMainRoom = 1, //父窗口是MianRoomEx
		ISGameRoom = 2, //父窗口是GameRoomEx
	}m_emParWndType;

	bank_op_normal m_bank_op_normal;
	bank_op_deposit m_bank_op_deposit;

	//类型按钮
	BzDui::CButtonUI* m_pSortBnt[3];

	//每个类型对应功能列表的容器
	BzDui::CContainerUI* m_pFunsCtn[3];

	//提交按钮
	BzDui::CButtonUI* m_pSubmitBnt;

	//转账记录表控件指针
	BzDui::CListUI* m_pRecordList;

	//存单记录表控件指针
	BzDui::CListUI* m_pDepList;

	struct TFunsNode
	{
		BzDui::CButtonUI* pFunsBnt;
		BzDui::CContainerUI* pFunsCtn;
		lpNotifyFun pNotifyFunPtr;
		bool bNeedSubmit;	//是否需要提交按钮
		BzDui::CStdString sBntName; //按钮的名称
		TFunsNode()
		{
			::memset(this,NULL,sizeof(TFunsNode));
		}
	};

	//每个类型的功能属性表
	list<TFunsNode> m_lstFuns[3];

//功能数据
private:

	//当前选择的功能
	TFunsNode* m_pSelFunNode;

	//银行里的存款
	__int64 m_i64BankMoney;

	//转换大写汉字用的缓存
	vector<int> digitArray;

	bool bFlag;

	//各游戏的钱包结点
	struct TGmWalletNode
	{
		UINT uNameID;			//游戏名称 ID 号码
		char szGameName[61];	//游戏名称
		__int64 i64Wallet;		//钱
		TGmWalletNode()
		{
			::memset(this,NULL,sizeof(TGmWalletNode));
		}
	};
	vector<TGmWalletNode> m_vecGmWallets;

	//用户ID
	DWORD m_dwOpUserID;

// 	//上次输入转账的目标用户ID
// 	CString m_strTransTargetID;
// 
// 	//上次输入转账的目标用户昵称
 	CString m_strTransTargetNickName;

	//转账成功
	bool m_bTransferSuccess; 

	//转账日志记录表
	std::map<int,MSG_GR_R_TransferRecord_t> m_mapRecord;

	//转账日志记录表显示位置
	int m_iRecDispPos;

	//创建存单成功
	bool m_bDepositSuccess;

	//存单记录表
	std::map<int,bank_op_deposit_node> m_mapDeposits;

	//计时用
	DWORD m_dwTickCount;

	//是否定时检测
	bool m_bCheckFrequence;
	int  m_iMinutes;

	//记录编辑框得焦点前的字符串，在killfocus时清除掉
	BzDui::CStdString m_strFocusTemp;

};


#endif
/*******************************************************************************************************/