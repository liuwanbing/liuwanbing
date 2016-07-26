#ifndef DLG_INSURE_COUNTER_HEAD_FILE
#define DLG_INSURE_COUNTER_HEAD_FILE

#pragma once

#include "MissionManager.h"
#include "PasswordControl.h"
#include "ShareControlHead.h"

//////////////////////////////////////////////////////////////////////////////////

//类说明
class CDlgInsureItem;
class CDlgInsureMain;
class CDlgInsureSave;

//////////////////////////////////////////////////////////////////////////////////

//操作方式
#define INSURE_SAVE					0									//存取方式
#define INSURE_TRANSFER				1									//转账方式

//////////////////////////////////////////////////////////////////////////////////

//银行信息
struct tagUserInsureInfo
{
	WORD							wRevenueTake;						//税收比例
	WORD							wRevenueTransfer;					//税收比例
	WORD							wServerID;							//房间标识
	SCORE							lUserScore;							//用户金币
	SCORE							lUserInsure;						//银行金币
	SCORE							lTransferPrerequisite;				//转账条件
};

//////////////////////////////////////////////////////////////////////////////////

//银行接口
interface IInsureCounterAction
{
	//查询事件
	virtual VOID PerformQueryInfo()=NULL;
	//存入事件
	virtual VOID PerformSaveScore(SCORE lSaveScore)=NULL;
	//取出事件
	virtual VOID PerformTakeScore(SCORE lTakeScore, LPCTSTR pszInsurePass)=NULL;
	//转账事件
	virtual VOID PerformTransferScore(BYTE cbByNickName, LPCTSTR pszNickName, SCORE lTransferScore, LPCTSTR pszInsurePass)=NULL;
};

//////////////////////////////////////////////////////////////////////////////////

//页面子类
class CDlgInsureItem
{
	//函数定义
public:
	//构造函数
	CDlgInsureItem();
	//析构函数
	virtual ~CDlgInsureItem();

	//功能函数
public:
	//获取金币
	SCORE GetUserControlScore(CWnd * pWndScore);

	//功能函数
public:
	//绘画数字
	VOID DrawNumberString(CDC * pDC, SCORE lScore, INT nXPos, INT nYPos);
	//转换字符
	VOID SwitchScoreString(SCORE lScore, LPTSTR pszBuffer, WORD wBufferSize);
	//转换字符
	VOID SwitchScoreFormat(SCORE lScore, UINT uSpace, LPTSTR pszBuffer, WORD wBufferSize);
};

//////////////////////////////////////////////////////////////////////////////////

//存取金币
class CDlgInsureSave : public CDialog
{
	//友元定义
	friend class CDlgInsureMain;

	//辅助变量
protected:
	bool							m_bSwitchIng;						//转换标志

	//对象指针
protected:
	CDlgInsureMain *				m_pDlgInsureMain;					//窗口指针
	tagUserInsureInfo *				m_pUserInsureInfo;					//用户信息

	//控件变量
public:
	CSkinEditEx						m_edScore;							//操作金币
	CSkinButton						m_btTakeScore;						//取款按钮
	CSkinButton						m_btSaveScore;						//存款按钮

	//其他控件
public:
	CSkinHyperLink					m_ForgetInsure;						//忘记密码
	CDlgInsureItem					m_DlgInsureItem;					//银行子项
	CPasswordControl				m_PasswordControl;					//用户密码

	//函数定义
public:
	//构造函数
	CDlgInsureSave();
	//析构函数
	virtual ~CDlgInsureSave();

	//重载函数
protected:
	//控件绑定
	virtual VOID DoDataExchange(CDataExchange * pDX);
	//创建函数
	virtual BOOL OnInitDialog();
	//确定按钮
	virtual VOID OnOK();
	//取消按钮
	virtual VOID OnCancel();

	//控件函数
protected:
	//输入信息
	VOID OnEnChangeScore();
	//存款按钮
	VOID OnBnClickedSaveScore();
	//取款按钮
	VOID OnBnClickedTakeScore();
	//忘记密码
	VOID OnBnClickedForgetInsure();

	//消息函数
protected:
	//绘画背景
	BOOL OnEraseBkgnd(CDC * pDC);
	//控件颜色
	HBRUSH OnCtlColor(CDC * pDC, CWnd * pWnd, UINT nCtlColor);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////////////

//转账金币
class CDlgInsureTransfer : public CDialog
{
	//友元定义
	friend class CDlgInsureMain;

	//辅助变量
protected:
	bool							m_bSwitchIng;						//转换标志

	//对象指针
protected:
	CDlgInsureMain *				m_pDlgInsureMain;					//窗口指针
	tagUserInsureInfo *				m_pUserInsureInfo;					//用户信息

	//控件变量
public:
	CSkinEditEx						m_edScore;							//操作金币
	CSkinEditEx						m_edNickName;						//用户昵称
	CSkinButton						m_btTransferScore;					//转账按钮

	//其他控件
public:
	CSkinHyperLink					m_ForgetInsure;						//忘记密码
	CDlgInsureItem					m_DlgInsureItem;					//银行子项
	CPasswordControl				m_PasswordControl;					//用户密码

	//函数定义
public:
	//构造函数
	CDlgInsureTransfer();
	//析构函数
	virtual ~CDlgInsureTransfer();

	//重载函数
protected:
	//控件绑定
	virtual VOID DoDataExchange(CDataExchange * pDX);
	//创建函数
	virtual BOOL OnInitDialog();
	//确定按钮
	virtual VOID OnOK();
	//取消按钮
	virtual VOID OnCancel();

	//控件函数
protected:
	//输入信息
	VOID OnEnChangeScore();
	//忘记密码
	VOID OnBnClickedForgetInsure();
	//转账按钮
	VOID OnBnClickedTransferScore();

	//消息函数
protected:
	//绘画背景
	BOOL OnEraseBkgnd(CDC * pDC);
	//控件颜色
	HBRUSH OnCtlColor(CDC * pDC, CWnd * pWnd, UINT nCtlColor);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedById();
	afx_msg void OnBnClickedByName();
};

//////////////////////////////////////////////////////////////////////////////////

//保险柜类
class SHARE_CONTROL_CLASS CDlgInsureMain : public CSkinDialog, public IInsureCounterAction
{
	//变量定义
protected:
	bool							m_bSaveMission;						//存款任务
	bool							m_bTakeMission;						//取款任务
	bool							m_bQueryMission;					//查询任务
	bool							m_bTransferMission;					//转账任务

	//查询变量
protected:
	bool							m_bInitInfo;						//银行信息
	BYTE							m_cbInsureMode;						//银行模式
	tagUserInsureInfo				m_UserInsureInfo;					//银行信息

	//其他控件
protected:
	CSkinTabCtrl					m_TabControl;						//操作选择
	CSkinButton						m_btQueryInfo;						//查询按钮

	//子项窗口
protected:
	CDlgInsureItem					m_DlgInsureItem;					//银行子项
	CDlgInsureSave					m_DlgInsureSave;					//存取子项
	CDlgInsureTransfer				m_DlgInsureTransfer;				//转账子项

	//函数定义
public:
	//构造函数
	CDlgInsureMain();
	//析构函数
	virtual ~CDlgInsureMain();

	//重载函数
protected:
	//控件绑定
	virtual VOID DoDataExchange(CDataExchange * pDX);
	//消息解释
	virtual BOOL PreTranslateMessage(MSG * pMsg);
	//创建函数
	virtual BOOL OnInitDialog();
	//确定按钮
	virtual VOID OnOK();

	//窗口函数
protected:
	//绘画消息
	virtual VOID OnDrawClientArea(CDC * pDC, INT nWidth, INT nHeight);

	//事件处理
public:
	//开始任务
	VOID OnMissionStart();
	//终止任务
	VOID OnMissionConclude();

	//功能函数
protected:
	//查询资料
	VOID QueryUserInsureInfo();
	//设置模式
	VOID SwitchInsureModeView(BYTE cbInsureMode);

	//消息函数
protected:
	//查询按钮
	VOID OnBnClickedQueryInfo();
	//类型改变
	VOID OnTcnSelchangeInsureMode(NMHDR * pNMHDR, LRESULT * pResult);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////////////

//广场银行
class SHARE_CONTROL_CLASS CDlgInsurePlaza : public CDlgInsureMain, public CMissionItem
{
	//变量定义
protected:
	BYTE                            m_cbByNickName;                       //昵称赠送
	SCORE							m_lScore;							//金币数目
	TCHAR							m_szNickName[LEN_NICKNAME];			//用户昵称
	TCHAR							m_szInsurePass[LEN_PASSWORD];		//银行密码

	//组件变量
protected:
	CMissionManager					m_MissionManager;					//任务管理

	//函数定义
public:
	//构造函数
	CDlgInsurePlaza();
	//析构函数
	virtual ~CDlgInsurePlaza();

	//执行函数
protected:
	//查询事件
	virtual VOID PerformQueryInfo();
	//存入事件
	virtual VOID PerformSaveScore(SCORE lSaveScore);
	//取出事件
	virtual VOID PerformTakeScore(SCORE lTakeScore, LPCTSTR pszInsurePass);
	//转账事件
	virtual VOID PerformTransferScore(BYTE cbByNickName, LPCTSTR pszNickName, SCORE lTransferScore, LPCTSTR pszInsurePass);

	//网络事件
private:
	//连接事件
	virtual bool OnEventMissionLink(INT nErrorCode);
	//关闭事件
	virtual bool OnEventMissionShut(BYTE cbShutReason);
	//读取事件
	virtual bool OnEventMissionRead(TCP_Command Command, VOID * pData, WORD wDataSize);
};

//////////////////////////////////////////////////////////////////////////////////

//房间银行
class SHARE_CONTROL_CLASS CDlgInsureServer : public CDlgInsureMain
{
	//变量定义
protected:
	ITCPSocket *					m_pITCPSocket;						//网络接口
	IClientUserItem *				m_pIMySelfUserItem;					//自己指针

	//函数定义
public:
	//构造函数
	CDlgInsureServer();
	//析构函数
	virtual ~CDlgInsureServer();

	//执行函数
protected:
	//查询事件
	virtual VOID PerformQueryInfo();
	//存入事件
	virtual VOID PerformSaveScore(SCORE lSaveScore);
	//取出事件
	virtual VOID PerformTakeScore(SCORE lTakeScore, LPCTSTR pszInsurePass);
	//转账事件
	virtual VOID PerformTransferScore(BYTE cbByNickName, LPCTSTR pszNickName, SCORE lTransferScore, LPCTSTR pszInsurePass);

	//功能函数
public:
	//配置银行
	VOID Initialization(ITCPSocket * pITCPSocket, IClientUserItem * pIMySelfUserItem);
	//银行消息
	bool OnServerInsureMessage(WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize);
};

//////////////////////////////////////////////////////////////////////////////////

#endif