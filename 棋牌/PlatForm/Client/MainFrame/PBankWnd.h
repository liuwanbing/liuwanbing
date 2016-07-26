#pragma once

#ifndef PBANKWND_HEAD_H
#define PBANKWND_HEAD_H

#include "StdAfx.h"
#include <map>
using namespace std;

#include "PBankWndEx.h"

class CPBankWnd : 
	public BzDui::CWindowWnd,
	public BzDui::INotifyUI,
	public BzDui::IMessageFilterUI,
	public BzDui::IListCallbackUI
{

public:

	CPBankWnd(CWnd* pParent,bool bInMainRoom = false);
	virtual ~CPBankWnd(void);

	LPCTSTR GetWindowClassName() const;

	UINT GetClassStyle() const;

	void OnFinalMessage(HWND hWnd);

	void Init();

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

	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

	LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);

	virtual LPCTSTR GetItemText(BzDui::CControlUI* pControl, int iIndex, int iSubItem);

	bool OnNetMessage(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket);

public:

	//加载常规控件皮肤
	void LoadNormalCtrlSkin();

	//显示容器页面
	void ShowPage(int nIndex);

	//初始化财富信息
	void InitMoneyInfo();

	//初始赠送操作UI
	void InitGiftInfo();

	//初始存取款操作UI
	void InitAccessInfo();

	//初始化日志操作UI
	void InitRecordInfo();

	//初始化修改银行密码UI
	void InitModifyPwdInfo();

	//转账金额小写转换为中文大写
	void OnEnChangeEtbconnoin();

	//存取款金额小写转换为中文大写
	void OnEnChangeMoney();

	//切换目标用户的输入类型
	void OnBnClickedRTransId();
	void OnBnClickedRTransNn();

	//切换存取类型
	void OnBnClickedAccessIn();
	void OnBnClickedAccessOut();

	///
	/// 帐号转账 的确定按钮响应函数
	/// @param void
	/// @return void
	void OnBnClickedAccountOK();

	//根据用户ID获取昵称,或根据用户昵称获取用户ID
	// @param pEdtCtrl 户ID/昵称输入控件
	// @param nType 操作类型，是ID获取昵称,还是昵称用户ID
	// @param nUserful 用途，是指这个功能所在有场合，如转账，充值等。
	// @return bool 是否操作成功
	// JianGuankun, 2012-7-30
	bool OnIDKillFocus(BzDui::CEditUI* pEdtCtrl,BYTE nType,BYTE nUserful);

	//输入用户ID/昵称
	void OnKillfocusTargetUser();

	///
	/// 银行 的确定按钮响应函数
	/// @param void
	/// @return void
	///
	void OnBnClickedBankOK();

	void OnBnClickedCoinAll();

	void OnBnClickedRecord();

	void RefreshRecordList();

	///
	/// 收到一条转账记录
	/// @param void
	/// @return void
	///
	void OnReceivedTransferRecord(MSG_GR_R_TransferRecord_t *pRecord);

	//记录翻到首页
	void OnBnClickedRecordFirstPage();

	//记录翻到尾页
	void OnBnClickedRecordRearPage();

	//记录翻上一页
	void OnBnClickedRecordPreviousPage();

	//记录翻下一页
	void OnBnClickedRecordNextPage();

	///
	/// 修改密码 的确定按钮响应函数
	/// @param void
	/// @return void
	///
	void OnBnClickedChPW();

private:

	//加载Combol的列表项
	void LoadComboItems(BzDui::CComboUI* pCtrl,LPCTSTR items[],int nums,int nSelect = 0);

	//根据文本设置选择列表项
	bool SelectComboItem(BzDui::CComboUI* pCtrl,LPCTSTR szText);

	//金额转中文大写
	void ChangeDigitToChinese(__int64 i64Num, CString &strResult);

public:	///< 外部调用的接口
	///
	/// 设置财富信息
	/// @param nBank 银行存款
	/// @param nWallet 手中现金
	/// @return void
	///
	void SetWealthInfor(__int64 i64Bank, __int64 i64Wallet);

	// PengJiLin, 2010-8-23, 根据用户ID获取的昵称
	// 设置获取的昵称
	void SetNickName(TCHAR* szNickName);

	///更新用户ID昵称的查询结果
	void UpdateNicknameIDResult(GetNickNameID_t* pNID);

	/// 转帐成功后更新银行数据
	/// @param Opt 0-表示转出给别人，1-表示由别人转入给自己
	/// @param nMoney 表示本次所操作的金币数
	void UpdateUserMoneyByTransfer(int OptType, __int64 i64Money);

	/// 存取钱后更新界面
	/// @param Opt 0-表示取出，1-表示存入
	/// @param nMoney 表示本次所操作的金币数
	void UpdateUserMoney(int OptType, __int64 i64Money);

	///刷新记录的页码
	void UpdateRecordPageState();

	///
	/// 提示
	/// @param pToCtrl 在某一控件上提示
	/// @param lpSecName ClientMessage.bcf中的段名
	/// @param lpKeyName ClientMessage.bcf中的键值
	/// @param lpDefault ClientMessage.bcf没有找到键值时，使用的默认值
	/// @return void
	///
	static void ShowTipMessage(BzDui::CControlUI* pToCtrl,LPCTSTR lpSecName,LPCTSTR lpKeyName,LPCTSTR lpDefault,...);

public:

	BzDui::CPaintManagerUI m_pm;

private:

public:

	//默认显示的页索引
	int m_nDefaultPage;

	//当前实例窗口句柄
	static HWND m_hInstanceWnd;

//定义控件
public:

	//页面容器
	BzDui::CContainerUI* m_pPageContainer[5];

	// 父窗口指针
	CWnd* m_pGameRoom;

	//父窗口的类型
	enum
	{
		IsMainRoom = 1, //父窗口是MianRoomEx
		ISGameRoom = 2, //父窗口是GameRoomEx
	}m_emParWndType;

	//记录表控件指针
	BzDui::CListUI* m_pRecordList;

	//翻页控件按钮
	BzDui::CButtonUI* m_pFirstPageBnt;
	BzDui::CButtonUI* m_pPrePageBnt;
	BzDui::CButtonUI* m_pNextPageBnt;
	BzDui::CButtonUI* m_pRearPageBnt;

private:

	//用户ID
	DWORD m_dwOpUserID;

	//手中的现金
	__int64 m_i64WalletMoney;	

	//银行里的存款
	__int64 m_i64BankMoney;

	vector<int> digitArray;
	bool bFlag;

	//充值的目标玩家，使用ID方式还是使用昵称方式
	bool m_bUseDestID;

	// PengJiLin, 2010-9-16, 是否使用昵称显示
	int m_iUseNickName;
	CButton m_Check_TransAll;

	// PengJiLin, 2010-8-23, 根据用户ID获取的昵称
	CString m_strNickName;
	bool m_bTransferSuccess; // 转账成功

	// PengJiLin, 2010-9-15
	CString m_strSaveID;

	// 存钱还是取钱，存钱为0，取钱为1
	UINT m_uCheckInOrOut;
	
	//转账日志记录表
	std::map<int,MSG_GR_R_TransferRecord_t> m_mapRecord;

	//转账日志记录表显示位置
	int m_iRecDispPos;

	//记录编辑框得焦点前的字符串，在killfocus时清除掉
	BzDui::CStdString m_strFocusTemp;
	

};


#endif
/*******************************************************************************************************/