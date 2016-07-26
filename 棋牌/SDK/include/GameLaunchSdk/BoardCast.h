#pragma once

#include "resource.h"
#include "Platform/AFCImage.h"
#include "Platform/AFCButton.h"
#include "SkinMgr.h"
// CBoardCast 对话框

class CBoardCast : public CDialog
{
	DECLARE_DYNAMIC(CBoardCast)

public:
	CBoardCast(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CBoardCast();
public:
	CGameImage				m_BackgroundImage3;	//小喇叭界面
	CGameImage				m_BackgroundImage2;	//大喇叭界面
	CNormalBitmapButtonEx	m_btCancel;
	CNormalBitmapButtonEx	m_btOk;
	CNormalBitmapButtonEx	m_btChangeColor;
	CWnd					*pParentWnd;
	int						m_nPropID;

	CString					m_strInput;
	UINT					m_uiInputLen;
	int						m_iBroadcastStyle;						//喇叭类型
	COLORREF				m_RGB;

	HBRUSH m_bkBrush;

    // PengJiLin, 2010-9-7, 踢人卡、防踢卡提示对话框，借用此类
    BOOL    m_bUseForOther;     // 是否是用作其他功能
    BOOL    m_bMouseInRect;     // 鼠标是否落在区域内
    BOOL    m_bIsTOut;          // 是否是踢人, TRUE = 踢人, FALSE = 防踢
    CString m_strInfo;          // 提示信息

    // PengJiLin, 2010-9-7, 模拟超链接的区域
    BOOL    m_bUseLink;         // 是否使用超链接
    int m_iLeft;
    int m_iTop;
    int m_iRight;
    int m_iBottom;

    // PengJiLin, 2010-9-7, 字体
    CFont m_fontCaption;

    // PengJiLin, 2010-9-9, 道具功能界面
    CGameImage				m_BackgroundImageProp;

private:
	CSkinMgr m_skinmgr;
// 对话框数据
	//enum { IDD = IDD_BOARDCAST };

public:
    // PengJiLin, 2010-9-9, 用于道具提示，bUseLink是否使用超链接
    void FuncForProp(BOOL bUseForOther, BOOL bUseLink, BOOL bIsTOut);
    void SetPropInfo(CString& strInfo);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持


	DECLARE_MESSAGE_MAP()
	afx_msg void OnChangeColor();

	virtual void OnOK();
public:
	afx_msg void OnPaint();

public:
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
public:
	virtual BOOL OnInitDialog();

public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnEnChangeEditInput();

    // PengJiLin, 2010-9-7
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);

public:
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

public:
	void SetValue(int iBroadcastMsg);
	//嘟嘟颜色
	COLORREF GetDuduRGB();

public:
	void LoadSkin();
};
