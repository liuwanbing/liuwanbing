#pragma once

/********************************************************************************************************/

//基础对话框类
class CBaseDialog : public CDialog
{
	//函数定义
public:
	//构造函数
	explicit CBaseDialog(UINT uTemplate, CWnd * pParent=NULL);
	//析构函数
	virtual ~CBaseDialog();

	//功能函数
public:
	//获取窗口
	bool SafeGetClientRect(UINT uID, CRect & ClientRect);
	//获取窗口
	bool SafeGetClientRect(CWnd * pControlWnd, CRect & ClientRect);
	//移动窗口
	bool SafeMoveWindow(UINT uID, int x, int y, int iWidth, int iHeight);
	//移动窗口
	bool SafeMoveWindow(CWnd * pControlWnd, int x, int y, int iWidth, int iHeight);

	DECLARE_MESSAGE_MAP()
};

/********************************************************************************************************/

//本地房间控制类
class CServiceControl : public CBaseDialog
{
	//变量定义
public:
	CListCtrl				m_StartList;					//启动房间列表
	CListCtrl				m_InstallList;					//已经安装的组件列表

	//函数定义 
public:
	//构造函数
	CServiceControl();
	//析构函数
	virtual ~CServiceControl();

	//重载函数
public:
	//DDX/DDV 支持
	virtual void DoDataExchange(CDataExchange * pDX); 
	//初始化函数
	virtual BOOL OnInitDialog();

	//消息映射
protected:
	DECLARE_MESSAGE_MAP()

	//消息函数
public:
	//位置变化函数
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//增加游戏组件
	afx_msg void OnBnClickedSetCom();
	//启动游戏大厅
	afx_msg void OnBnClickedStart();
	//停止游戏房间
	afx_msg void OnBnClickedStop();
	//刷新游戏组件列表
	afx_msg void OnBnClickedReLoad();
	//更新游戏组件
	afx_msg void OnBnClickedUpdateCom();
	//安装游戏组件库
	afx_msg void OnBnClickedSetup();
	//修改游戏组件
	afx_msg void OnBnClickedMotif();

	afx_msg void OnBnClickedRemove();
	//启动所有房间
	afx_msg void OnBnClickedStartAll();
	//停止所有房间
	afx_msg void OnBnClickedStopAll();

	afx_msg void OnBnClickedDelete();

	afx_msg void OnBnClickedChange();

	afx_msg void OnBnClickedInfo();

	afx_msg void OnBnClickedManage();

	afx_msg void OnBnClickedReloadRoomlist();

public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};

/********************************************************************************************************/


