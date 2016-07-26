#pragma once

#include "Stdafx.h"
#include "Resource.h"

//游戏设置信息类
class CGameDaoSHiDlg : public CCoolView//CGameFaceGo
{

	//变量定义
public:
	enum				{ IDD = IDD_DAO_SHI };			//指导费确认对话框

	//控件变量
public:	
	CNormalBitmapButtonEx			m_btOk;								//确定按钮
	CNormalBitmapButtonEx			m_btCancel;							//取消按钮
	CWnd*							m_pPP;
	CGameImage						m_bk;
	//函数定义
public:
	//构造函数
	CGameDaoSHiDlg(CWnd * pParent=NULL);
	//析够函数
	virtual ~CGameDaoSHiDlg();
	//取消按钮
	virtual void OnCancel();
	//确定按钮
	virtual void OnOK();	
	//设置对话框的大小
	void AutoSize();
	//设置对话框信息
	void SetInfo(CString &str);

protected:
	// DDX/DDV 支持
	virtual void DoDataExchange(CDataExchange* pDX);    
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
public:	
};
