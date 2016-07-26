#pragma once
#include "afxwin.h"
#include "SkinListCtrl.h"
#include <map>
#include "SkinMgr.h"

/// 存取钱用的消息定义
const int WM_CHECK_MONEY	= WM_USER + 520;
const int WM_TRANSFER_MONEY	= WM_USER + 521;
const int WM_CHANGE_PASSWD	= WM_USER + 522;
const int WM_TRANSFER_RECORD= WM_USER + 523;
const int WM_CHECK_MONEY2   = WM_USER + 524; //新版银行存取款

// 根据用户ID获取昵称 lxl, 2010-11-17
const int WM_BANK_GET_NICKNAME_ONID = WM_USER + 525;

// lxl 2010-11-19  金额转中文大写功能
const int DIGITSIZE = 10;	//数字中文大写
const int UNITSIZE = 19;	//金额最大位数

static char* szDigit[DIGITSIZE] = {"零", "壹", "贰", "叁", "肆", "伍", "陆", "柒", "捌", "玖"}; //中文大写数字
static char* szUnit[UNITSIZE] = {"", "拾", "佰", "仟", "万", "拾", "佰", "仟", "亿", "拾","佰","仟","万","拾","佰","仟","万","拾","佰"};	  //中文大写单位


/// 存取钱
/// 用来与父窗口进行通讯的结构
struct CheckMoney_t
{
	UINT uType;			///< 存或取，存为0，取为1
	__int64 i64Money;	///< 要存取的金额
	TCHAR szPWD[32];	///< 密码，未加密
	CheckMoney_t()
	{
		ZeroMemory(this, sizeof(CheckMoney_t));
	}
};
/// 转帐
/// 用来与父窗口进行通讯的结构
struct TransferMoney_t
{
	UINT uDestUserID;	///< 要转给的目标用户ID
	__int64 i64Money;			///< 要存取的金额
	TCHAR szPWD[32];	///< 密码，未加密
	TCHAR szDestNickName[50]; ///目标昵称
	bool bUseDestID;
	TransferMoney_t()
	{
		ZeroMemory(this, sizeof(TransferMoney_t));
	}
};

// PengJiLin, 2010-8-23, 根据用户ID获取昵称
struct GetNickNameOnID_t
{
    UINT    uUserID;        // 用户ID
    TCHAR   chNickName[61]; // 昵称

    GetNickNameOnID_t()
    {
        ZeroMemory(this, sizeof(GetNickNameOnID_t));
    }
};

/// 修改密码
/// 用来与父窗口进行通讯的结构
struct ChangePasswd_t
{
	TCHAR szPwdOld[32];
	TCHAR szPwdNew[32];
	ChangePasswd_t()
	{
		ZeroMemory(this, sizeof(ChangePasswd_t));
	}
};

// CPBankDlg 对话框

class CPBankDlg : public CDialog
{
	DECLARE_DYNAMIC(CPBankDlg)

	HBRUSH m_bkBrush;
public:
	CPBankDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CPBankDlg();
	
// 对话框数据
	//enum { IDD = IDD_BANK };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

public:
	virtual BOOL OnInitDialog();
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

public:
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnBnClickedInfor();
	afx_msg void OnBnClickedTrans();
	afx_msg void OnBnClickedRecord();
	afx_msg void OnBnClickedSafe();
	afx_msg void OnBnClickedPassWord();

	afx_msg void OnBnClickedSave();
	afx_msg void OnBnClickedGet();

	afx_msg void OnBnClickedRTransId();
	afx_msg void OnBnClickedRTransNn();

	///
	/// 帐号转账 的确定按钮响应函数
	/// @param void
	/// @return void
	afx_msg void OnBnClickedAccountOK();

	///
	/// 修改密码 的确定按钮响应函数
	/// @param void
	/// @return void
	///
	afx_msg void OnBnClickedChPW();
	
	///
	/// 银行 的确定按钮响应函数
	/// @param void
	/// @return void
	///
	afx_msg void OnBnClickedBankOK();

	///
	/// 银行 的激活按钮响应函数
	/// @param void
	/// @return void
	///
	afx_msg void OnBnClickedBankActive();

	///
	/// 银行 的续费按钮响应函数
	/// @param void
	/// @return void
	///
	afx_msg void OnBnClickedBankRecharge();
	
	// 根据用户ID获取昵称 lxl, 2010-11-17
    afx_msg void OnIDKillFocus();

	// lxl 2010-11-19  转账金额小写转换为中文大写
	afx_msg void OnEnChangeEtbconnoin();
	// lxl 2010-11-19  存取款金额小写转换为中文大写
	afx_msg void OnEnChangeMoney();

	afx_msg void OnBnClickedCoinAll();

protected:
	void ShowInterface();
	DWORD GetBackGroundColor();
	virtual void OnOK();

private:
	CWnd *m_pGameRoom;		/// 父窗口指针
	Image *m_pbgImg;
	CGameImage		m_bkimage;
	CNormalBitmapButtonEx m_BtClose;
	CNormalBitmapButtonEx m_btInfor;
	CNormalBitmapButtonEx m_bnTrans;
	CNormalBitmapButtonEx m_bnRecord;
	CNormalBitmapButtonEx m_bnSafe;
	CNormalBitmapButtonEx m_bnPW;
	CNormalBitmapButtonEx m_btnRefresh;
	CNormalBitmapButtonEx m_btnRcoClose;
	
	CNormalBitmapButtonEx m_btnTranOK;
	
	CNormalBitmapButtonEx m_btnBok;
	CNormalBitmapButtonEx m_btnBactive;
	CNormalBitmapButtonEx m_btnBcharge;

	CNormalBitmapButtonEx m_btnChOK;
	CNormalBitmapButtonEx m_btnChClose;

	CNormalBitmapButtonEx m_btnCoinAll;

	CSkinListCtrl	m_SkinList;

	DECLARE_MESSAGE_MAP()

public:	///< 外部调用的接口
	///
	/// 设置财富信息
	/// @param nBank 银行存款
	/// @param nWallet 手中现金
	/// @return void
	///
	void SetWealthInfor(__int64 i64Bank, __int64 i64Wallet);

	/// 存取钱后更新界面
	/// @param Opt 0-表示取出，1-表示存入
	/// @param nMoney 表示本次所操作的金币数
	void UpdateUserMoney(int OptType, __int64 i64Money);

	/// 转帐成功后更新银行数据
	/// @param Opt 0-表示转出给别人，1-表示由别人转入给自己
	/// @param nMoney 表示本次所操作的金币数
	void UpdateUserMoneyByTransfer(int OptType, __int64 i64Money);
	///
	/// 设置 帐号转账 下的金币数
	/// @param void
	/// @return void
	///
	void SetAccountInfor();

	///
	/// 设置 插入转账数据
	/// @param void
	/// @return void
	///
	void InsertRecordList();

	///
	/// 刷新 转账数据
	/// @param void
	/// @return void
	///
	void RefreshRecordList();
	///
	/// 收到一条转账记录
	/// @param void
	/// @return void
	///
	void OnReceivedTransferRecord(MSG_GR_R_TransferRecord_t *pRecord);

   
    // 设置获取的昵称 lxl  2010-11-17
    void SetNickName(TCHAR* szNickName);
    CString m_strNickName;   
	CString m_strSaveID;
    int m_iUseNickName;				// 是否使用昵称显示

	XLogic	*m_pLogic;

private:
	void ShowMoneyBank();

	int		m_nCurPage;
	UINT	m_uCheckInOrOut;	///< 存钱还是取钱，存钱为0，取钱为1
	bool	m_bUseDestID;
	__int64	m_i64WalletMoney;		///< 手中的现金
	__int64	m_i64BankMoney;		///< 银行里的存款
	std::map<int, MSG_GR_R_TransferRecord_t> m_mapTransferRecord;

	void ChangeDigitToChinese(__int64 i64Num, CString &result); //add by lxl 2010-11-19 金额转中文大写
	vector<int> digitArray;
	bool bFlag;
	CSkinMgr m_skinmgr;

public:
	void LoadSkin();
};
