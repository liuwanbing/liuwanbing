#pragma once
#ifndef GETPOINT_HEAD_H
#define GETPOINT_HEAd_H

#include "PublicDefine.h"
#include "AFCImage.h"
#include "AFCButton.h"
#include "SkinMgr.h"

// CGetPoint 对话框

class EXT_CLASS  CGetPoint : public CDialog
{
	DECLARE_DYNAMIC(CGetPoint)

public:
	CGetPoint(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CGetPoint();
	int		m_SerPoint;
	CWnd			* m_pSend;
	int		m_iInWhere;					//0大厅,1房间

	HBRUSH m_bkBrush;
// 对话框数据

#ifndef IDD_GETPOINT
	#define IDD_GETPOINT 2068
#endif
	//enum { IDD = IDD_GETPOINT };

protected:

	CGameImage	m_bkimage;	

	CNormalBitmapButtonEx	m_btcancel;
	CNormalBitmapButtonEx	m_btok;



	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnPaint();
public:
	virtual BOOL OnInitDialog();
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
public:
	afx_msg void OnBnClickedOk();
public:
	void SetPoint(DWORD dwPoint);
private:
	CSkinMgr	m_skinmgr;
};
#endif