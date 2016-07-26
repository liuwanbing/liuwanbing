#pragma once
#ifndef ROOMSETDLG_HEAD_H
#define ROOMSETDLG_HEAD_H

#include "StdAfx.h"
#include "Resource.h"
#include "..\UILibrary2003\CconfigDlg.h" 
//#include "NewControl\\MacButtons.h"

//接受邀请模式
#define INVALI_ALL					0			//接受所有邀请
#define INVALI_ONLY_FRIEND			1			//只接受朋友邀请
#define INVALI_NO					2			//不接收任何邀请

/*******************************************************************************************************/

//按钮子项
struct StructButtonItem
{
	UINT											uButtonID;						//按钮 ID
	CString											strButtonText;					//按钮标题
};

//设置按钮窗口
class CAFCButtonBar : public CWnd
{
	//变量定义
protected:
	INT_PTR											m_uHotItem;						//激活项
	INT_PTR											m_uActiveItem;					//激活项
	CArray<StructButtonItem,StructButtonItem>		m_ButtonInfoArray;				//按钮数组

	//函数定义
public:
	//构造函数
	CAFCButtonBar();
	//析构函数
	virtual ~CAFCButtonBar();

	//重载函数
public:
	//消息解释
	virtual BOOL PreTranslateMessage(MSG * pMsg);

	DECLARE_MESSAGE_MAP()

	//功能函数
public:
	//加入按钮
	INT_PTR AddButton(UINT uButtonID, TCHAR * szButtonTitle);
	//删除按钮
	INT_PTR DeleteButton(UINT uButtonID);
	//设置激活项
	void SetActiveButton(UINT uButtonID);

	//内部函数
private:
	//按键测试
	INT_PTR HitTest(CPoint MousePoint);
	//绘画子项
	void DrawButtonItem(CDC * pDC, CRect & rcDraw, INT_PTR uIndex);

	//消息函数
protected:
	//重画函数
	afx_msg void OnPaint();
	//鼠标光标消息
	afx_msg BOOL OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT message);
	//鼠标右键按下
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};

/*******************************************************************************************************/

//设置对话框基础类
class CBaseSetDlg : public CGameFaceGo
{
	//函数定义
public:
	//构造函数
	CBaseSetDlg(); 
	//析构函数
	virtual ~CBaseSetDlg();

	DECLARE_MESSAGE_MAP()

	//重载函数
public:
	//确定
	virtual void OnOK() { return; }
	//取消
	virtual void OnCancel() { GetParent()->PostMessage(WM_CLOSE,0,0); }

	//接口函数
public:
	//保存变量
	virtual bool SaveSetting()=0;

	//消息函数
public:
	//重画函数
	afx_msg void OnPaint();
};

/*******************************************************************************************************/

//规则对话框类
class CGameRuleSetDlg : public CBaseSetDlg
{


	HBRUSH m_bkBrush;
	//变量定义
public:
	BYTE								m_bSaveTalk;						//保存记录
	BYTE								m_bShowUser;						//显示用户
	BYTE								m_bShowMessage;						//显示信息

public:
	BYTE								m_bSameIP;							//相同 IP
	BYTE								m_bSameIPNumber;					//不于前几位IP相同的玩家游戏
	BYTE								m_bPass;							//设置密码
	BYTE								m_bLimitCut;						//限制断线
	BYTE								m_bLimitPoint;						//限制经验值
	BYTE								m_bCutPercent;						//逃跑率
	BYTE								m_bInvaliMode;						//邀请模式
	TCHAR								m_szPass[61];						//桌子密码
	long int							m_dwLowPoint;						//最低经验值
	long int							m_dwHighPoint;						//最高经验值 

	//控件变量
public:
	CTrueEdit							m_CutInput;							//逃跑率
	CTrueEdit							m_HightPoint;						//最高经验值
	CTrueEdit							m_LessPoint;						//最低经验值
	CTrueEdit							m_PassWord;							//密码框
	CGameImage							m_bk;//背景

	//函数定义
public:
	//构造函数
	CGameRuleSetDlg(); 
	//析构函数
	virtual ~CGameRuleSetDlg();

	DECLARE_MESSAGE_MAP()

	//重载函数
public:
	//初始化函数
	virtual BOOL OnInitDialog();

	//DDX/DDV 支持
	virtual void DoDataExchange(CDataExchange * pDX);    
	//保存变量
	virtual bool SaveSetting();

	//消息函数
public:
	//得到焦点
	afx_msg void OnSetFocus(CWnd * pOldWnd);
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
public:
	afx_msg void OnPaint();
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
};

/*******************************************************************************************************/

//房间对话框类
class CRoomRuleSetDlg : public CBaseSetDlg
{
	//变量定义
public:
	BYTE								m_bSaveTalk;						//保存记录
	BYTE								m_bShowUser;						//显示用户
	BYTE								m_bShowMessage;						//显示信息

	//函数定义
public:
	//构造函数
	CRoomRuleSetDlg(); 
	//析构函数
	virtual ~CRoomRuleSetDlg();

	DECLARE_MESSAGE_MAP()

	//重载函数
public:
	//初始化函数
	virtual BOOL OnInitDialog();
	//DDX/DDV 支持
	virtual void DoDataExchange(CDataExchange * pDX);    
	//保存变量
	virtual bool SaveSetting();
};

/*******************************************************************************************************/

//设置对话框
class CRoomSetDlg : public CLoadFileBmp
{
	//变量定义
public:
	BYTE								m_bSaveTalk;						//保存记录
	BYTE								m_bShowUser;						//显示用户
	BYTE								m_bShowMessage;						//显示信息
	//变量定义
protected:
	int										m_iActiveIndex;					//激活位置
	CBaseSetDlg								* m_pActiveSet;					//激活窗口

	//控件变量
public:
	CGameRuleSetDlg							m_GameRuleSet;					//规则设置
	CNormalBitmapButtonEx							m_BtOK;							//确定按钮
	CNormalBitmapButtonEx							m_BtCancel;						//取消按钮
	CNormalBitmapButtonEx							m_BtClose;						//JMod-关闭按钮-20090515
	CGameImage								m_bk; 
	CconfigDlg                              m_configDlg; //加载底图类. 2012.10.17 yyf
	CRect							m_rect;//版权信息
	//函数定义
public:
	//构造函数
	CRoomSetDlg(CWnd * pParent=NULL);
	//析构函数
	virtual ~CRoomSetDlg();

	DECLARE_MESSAGE_MAP()

	//重载函数
protected:
	//数据绑定函数
	virtual void DoDataExchange(CDataExchange * pDX);
	//初始化函数
	virtual BOOL OnInitDialog();
	//确定
	virtual void OnOK();

	//功能函数
protected:
	//调整控件 
	void FixControlSize(int iWidth, int iHeight);

	//消息函数
public:
	//位置变化
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//按动按钮
	LRESULT OnHitButton(WPARAM wparam, LPARAM lparam);
	//保存变量
	virtual bool SaveSetting();
public:
	afx_msg void OnPaint();
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
public:
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
public:
	afx_msg void OnBnClickedCancel2();
	  // 设置窗口圆角 2012.10.17 yyf
	  void SetWndRgn(void);
};

#endif
/*******************************************************************************************************/
