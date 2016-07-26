#pragma once
#ifndef USERPROP_HEAD_H
#define USERPROP_HEAD_H
#include "Resource.h"
#include "afcbutton.h"
#include "afcfunction.h"

//游戏设置对话框类 
class EXT_CLASS CUserProp : public CDialog//CGameFaceGo
{
//变量定义
public:
	CNormalBitmapButtonEx				m_Cancel;
	CNormalBitmapButtonEx				m_Ok;
	enum								{ IDD = IDD_PROP };

	CWnd								*m_pParent;
	CGameImage							m_bk;			//背景图
	TCHAR								m_szName[MAX_PEOPLE][21];
	int									m_iCurCount;
	int									m_iCurDest;
	int									m_iCurIndex;							//当前道俱类型
	CRect								m_iRect[2];
	//函数定义 
public:
	//构造函数
	CUserProp(CWnd * pParent=NULL);
	//析构函数
	virtual ~CUserProp();
	//初始化对话框
	virtual BOOL OnInitDialog();
	//增加用户昵称
	virtual void AddUser(TCHAR *szUserName);
	//初始化
	void InitUserList();
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

	//DDX/DDV 支持
	virtual void DoDataExchange(CDataExchange* pDX);    
	//
	virtual void OnOK();
	//
	afx_msg void OnPaint();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
public:
	afx_msg void OnMove(int x, int y);
};

#endif