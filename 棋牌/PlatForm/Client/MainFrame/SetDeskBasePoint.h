#pragma once
#ifndef SETDESKBASEPOINT_HEAD_H
#define SETDESKBASEPOINT_HEAD_H

#include "resource.h"
#include "SkinMgr.h"
// CSetDeskBasePoint 对话框

class CSetDeskBasePoint : public CDialog
{
	DECLARE_DYNAMIC(CSetDeskBasePoint)

public:
	int										m_iBasePoint;
	CNormalBitmapButtonEx					m_BtOK;								//应用按钮
	CNormalBitmapButtonEx					m_BtCancel;							//取消按钮
	CGameImage								m_bk;
	CSetDeskBasePoint(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSetDeskBasePoint();

	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	int GetBasePoint();

// 对话框数据
	enum { IDD = IDD_SETDESKBPDLG };

protected:
	HBRUSH m_bkBrush;
	


	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//确定函数
	virtual void OnOK();
	//初始化函数
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

private:
	CSkinMgr m_skinmgr;

public:
	LRESULT OnExchangeSkin(WPARAM wpara,LPARAM lpara);

};

#endif