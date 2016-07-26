#pragma once
#include "Resource.h"
#include "TransparentStatic.h"

// CLeaveMessage 对话框

class CLeaveMessage : public CGameFaceGo
{
	DECLARE_DYNAMIC(CLeaveMessage)

public:
	CLeaveMessage(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CLeaveMessage();

// 对话框数据
	enum { IDD = IDD_THING_MESSAGE };

	//设置信息
	void SetLeaveMessage(TCHAR * szMessage, TCHAR * szUserName);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
protected:
	CTransparentStatic m_NickName;       // 有事离开玩家的昵称
	CNormalBitmapButtonEx m_btnOK;          // 同意按钮
	CNormalBitmapButtonEx m_btnCancel;      // 不同意
	CGameImage    m_imgBackground;          // 背景图片
public:
	virtual BOOL OnInitDialog();

	//取消函数
	virtual void OnCancel();
	//确定函数
	virtual void OnOK();
public:
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags,CPoint point);
public:
	virtual BOOL Create(UINT nIDTemplate, CWnd* pParentWnd = NULL);
};
