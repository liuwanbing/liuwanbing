#pragma once

#include "StdAfx.h"

/// 游戏信息类
class CGameNoteWnd : public CWnd
{
	/// 变量定义
protected:
	CFont				m_Font;						///< 显示字体
	CString				m_strMessage;				///< 字符信息
	CGameImage			m_BackBmp;					///< 背景

	/// 函数定义
public:
	/// 构造函数
	CGameNoteWnd();
	/// 析构函数
	virtual ~CGameNoteWnd();
	/// 自动调整大小
	void AutoSize();
	/// 设置信息
	void SetMessage(TCHAR * szMessage);

	/// 消息函数
public:
	/// 重画函数
	afx_msg void OnPaint();

	DECLARE_MESSAGE_MAP()
};
