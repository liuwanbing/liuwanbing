#pragma once
#ifndef LOGOSELECT_HEAD_H
#define LOGOSELECT_HEAD_H
#include "StdAfx.h"
#include "GamePlace.h"

//按动头像消息
#define WM_HIT_LOGO_MESSTION		WM_USER+215								//按动头像消息

//头像选择类
class CLogoSelect : public CDialog
{
	//变量定义
protected:
	bool							m_bVisible;								//是否可视 
	int								m_iPos;									//当前位置
	int								m_iWidthCount;							//行数目
	UINT							m_uSelectIndex;							//选择索引
	HCURSOR							m_hMouseCursor;							//小手形状的鼠标光标

	//函数定义
public:
	//构造函数
	CLogoSelect();
	//析构函数
	virtual ~CLogoSelect();
	//确定函数
	virtual void OnOK() { return; }
	//取消函数
	virtual void OnCancel();
	//初始化函数
	virtual BOOL OnInitDialog();
	//显示窗口
	void SetVisible() { m_bVisible=true; };

	//消息映射
protected:
	DECLARE_MESSAGE_MAP()

	//辅助函数
protected:
	//绘画头像
	void DrawItem(CDC * pDC, UINT uIndex, int iXPos, int iYPos, bool bSelect);
	//按键测试
	UINT HitItemTest(CPoint MousePoint);

	//消息函数
public:
	//重画消息
	afx_msg void OnPaint();
	//失去焦点消息
	afx_msg void OnKillFocus(CWnd * pNewWnd);
	//鼠标移动消息
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//鼠标按下消息
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//设置光标消息
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	//滚动条信息
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar * pScrollBar);
	//鼠标滚轮消息
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	//键盘按下
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//位置变化消息
	afx_msg void OnWindowPosChanging(WINDOWPOS* lpwndpos);
};

#endif