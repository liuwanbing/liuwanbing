#pragma once
#ifndef CHATCONTROL_HEAD_H
#define CHATCONTROL_HEAD_H
#include "UserListCtrl.h"

#include "UseBoardCastWnd.h"
#include "SkinMgr.h"

//聊天控制框
class CChatControl : public CWnd
{
	//变量定义
public:
	CToolTipCtrl					m_ToolTip;							//工具提示
	CComboBox						m_InputMessage;						//输入信息
	CNormalBitmapButtonEx			m_BtSend;							//发送按钮
	CNormalBitmapButtonEx			m_BtExpression;						//表情按钮
	CNormalBitmapButtonEx			m_BtBoardCast;						//喇叭
	int								m_iSendY,m_iSendX;
	int								m_iExpX,m_iExpY;
	COLORREF						m_bkColor;
	CGameImage						m_send,m_exp, m_boardcast, m_bkImg;

	CUseBoardCastWnd				*m_BoardCastWnd;
	//函数定义
public:
	//构造函数
	CChatControl();
	//析构函数
	virtual ~CChatControl();

	DECLARE_MESSAGE_MAP()

	//功能函数
public:
	//获取聊天控件
	CComboBox * GetTalkInput() { return &m_InputMessage; };

	//重载函数
public:
	//消息解释
	virtual BOOL PreTranslateMessage(MSG * pMsg);
	
	void SetBkColor(COLORREF color);
	
	void SetImage(TCHAR path1[],TCHAR path2[], TCHAR path3[]);
	
	void SetImage(TCHAR bkimg[],TCHAR path1[],TCHAR path2[], TCHAR path3[]);
	//消息函数
public:
	//建立通知函数
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//位置变化
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//重画函数
	afx_msg void OnPaint();

	//按钮消息
public:
	//发送按钮
	void OnBnClickedSend();
	//表情按钮
	void OnBnClickedExpression();
	//动作按钮
	void OnBnClickedAction();
	//属性按钮
	void OnBnClickedAttrib();
	//加载聊天配置文件
	void ReLoadTalkINI();
	//加载界面
	void LoadSkin();

	//喇叭按钮
	void OnBnClickedBoardCast();


	void UseProp_Buy(int nPropIndex);
private:
	CSkinMgr m_skinmgr;
private:
	LRESULT OnExchangeSkin(WPARAM wpara,LPARAM lpara);

public:
	CWnd*	m_pGameRoom;
	void SetGameRoomWnd(CWnd* pWnd);
};
#endif
