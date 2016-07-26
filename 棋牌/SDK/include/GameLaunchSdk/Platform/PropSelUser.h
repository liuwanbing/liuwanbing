#pragma once

#include "publicdefine.h"
#include "ClientComStruct.h"
//#include "AFCResource.h"
//#include "AFCImage.h"
#include "afxwin.h"
#include "AFCButton.h"
#include "..\SkinMgr.h"
// CPropSelUser 对话框
#define UM_USEITEM_USERSELECTED		WM_USER+3421
class CPropSelUser : public CDialog
{
	DECLARE_DYNAMIC(CPropSelUser)
	COLORREF m_bkColor;													//背景颜色
	COLORREF m_txtColor;												//提示文字颜色，现用的是黑色，保留
	HBRUSH m_bkBrush;
public:
	CPropSelUser(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CPropSelUser();
	int		dwUsedPropID;												//最后选择的用户ID
	int		dwLocalUserID;												//当前的用户ID
	UserItemStruct					* m_pUserItem[MAX_PEOPLE];			//用户信息
	int		maxUser;													//显示最多多少个用户
	CRect   userRect[MAX_PEOPLE];										//各个用户的位置
	Image *userImg[MAX_PEOPLE];											//各个用户的头像
	Image *imgDialog;													//背景
	int		userID[MAX_PEOPLE];											//各个用户的ID号
	CString userName[MAX_PEOPLE];										//各个用户的昵称
	CString propItemName;												//要使用的道具名称

// 对话框数据
	//enum { IDD = IDD_PROP_SEL_USER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
public:
	void drawSkin(void);
public:
	afx_msg void OnPaint();
public:
	void resetInformation(void);
protected:
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
public:
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
public:
	void Init(void);
public:
	CNormalBitmapButtonEx m_bnClose;
public:
	afx_msg void OnBnClickedClose();
private:
	CSkinMgr m_skinmgr;
public:
	void LoadSkin();
};
