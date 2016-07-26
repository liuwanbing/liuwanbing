#pragma once

#ifndef CGAMEENDVIEW_HEAD_FILE
#define CGAMEENDVIEW_HEAD_FILE

#include "PublicDefine.h"
#include "AFCResource.h"

/*******************************************************************************************************/

//游戏结束视图
class EXT_CLASS CPalatesNeck/*C-GameEndView*/ : public CWnd
{
	//变量定义
protected:
	long int						m_dwGameTax;						//游戏税收
	long int						m_dwPoint[MAX_PEOPLE];				//游戏得分
	CString							m_strName[MAX_PEOPLE];				//用户名字
	CSize							m_WndSize;							//窗口大小
	CGameImage						m_BackBmp;							//背景图案
	const bool						m_bShowTax;							//是否税收
	const BYTE						m_bGamePeople;						//游戏人数
	CFont							m_Font;

	//函数定义
public:
	//构造函数
	CPalatesNeck(BYTE bGamePeople, bool bShowTax);
	//析构函数
	virtual ~CPalatesNeck();

	//功能函数
public:
	//获取大小函数
	void GetWndSize(SIZE & Size);
	//设置信息
	void SetGamePoint(BYTE bDeskStation, TCHAR * szUserName, long int dwPoint);
	//设置税收
	void SetTax(long int dwGameTax);

	//消息函数
protected:
	//重画函数
	afx_msg void OnPaint();
	//建立消息
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};

/*******************************************************************************************************/

#endif