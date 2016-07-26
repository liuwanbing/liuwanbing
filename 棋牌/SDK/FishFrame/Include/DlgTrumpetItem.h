#ifndef SINGLE_PRO_ITEM_CONTROL_HEAD
#define SINGLE_PRO_ITEM_CONTROL_HEAD

#pragma once

#include "GamePropertyHead.h"


//////////////////////////////////////////////////////////////////////////
//啦八道具
class  GAME_PROPERTY_CLASS  CDlgTrumpetItem : public CSkinDialog,public IExpressionSink
{
	//变量定义
protected:
	int								m_nMaxChar;						//最大个数
	WORD                            m_wRadioSelected;               //单选按钮  
	COLORREF						m_crChatTX;						//字体颜色
	CString							m_strInuptChat;					//输入信息
	CString							m_strPropertyInfo;				//道具信息

	//控件变量
protected:
	CExpressionControl *			m_pExpressionControl;			//表情窗口
	CEdit							m_InputChat;					//聊天框
	CSkinButton						m_btOK;							//确定按钮
	CSkinButton						m_btCancel;						//关闭按钮
	CSkinButton						m_btExpression;					//表情按钮
	CSkinButton						m_btColorSet;					//颜色设置

	//接口变量
protected:
	IGamePropertyUseSink *          m_pGameTrumpetSink;             //回调接口
	IClientUserItem  *              m_pIClientUserItem;             //用户接口

	//函数定义
public:
	//构造函数
	CDlgTrumpetItem(); 

	//析构函数
	virtual ~CDlgTrumpetItem();

	//绑定函数
protected:
	//控件绑定
	virtual void DoDataExchange(CDataExchange* pDX);    

	//表情接口
public:
	//选择事件
	virtual VOID OnExpressionSelect(CExpression * pExpression, tagExpressionInfo * pExpressionInfo);

	//辅助函数
public:
	//设置接口
	bool SetGameTrumpetSink(IUnknownEx * pIUnknownEx);
	//更新界面
	void UpdateControlSurface();
	//获取内容
	void GetTrumpetContent(LPTSTR pszTrumpetContent);
	//获取颜色
	COLORREF GetTrumpetColor() { return m_crChatTX; };
	//显示窗口
	void ShowTrumpetWindow(CWnd * pParentWnd,IClientUserItem  * pIClientUserItem);


	//消息处理
protected:
	//初始框架
	virtual BOOL OnInitDialog();
	//颜色按钮
	afx_msg void OnBnClickedColorSet();
	//表情按钮
	afx_msg void OnBnClickedExpression();
	//喇叭按钮
	afx_msg void OnBnClickedTrumpet();
	//喇叭按钮
	afx_msg void OnBnClickedTyphon();
	//确认消息
	afx_msg void OnBnClickedOk();
	//控件颜色
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//绘画函数
	afx_msg void OnPaint();

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif
