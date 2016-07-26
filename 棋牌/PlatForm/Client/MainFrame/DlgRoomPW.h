#pragma once
#include "Stdafx.h"
#include "resource.h"
#include "afxwin.h"
#include "SkinMgr.h"
///@author yc.yan
///@version 1.0
///@date 2010.3.19
///

///
///CDlgRoomPW 对话框
///
class CDlgRoomPW : public CDialog
{
	DECLARE_DYNAMIC(CDlgRoomPW)

public:
	CDlgRoomPW(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgRoomPW();

// 对话框数据
	enum { IDD = IDD_GAME_ROOM_PW };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	/// 画图
	afx_msg void OnPaint();
	/// 退出按键响应
	afx_msg void OnBnClickedQuit();
	DECLARE_MESSAGE_MAP()
	virtual void OnOK();
public:
	virtual BOOL OnInitDialog();
private:
	/// 加载皮肤
	void LoadSkin();
	/// 初始化按键皮肤
	void InitBtnSkin();
public:
	TCHAR m_szPassWord[13]; ///<密码
	TCHAR m_szPassWordConfirm[13];///<密码
private:
	Bitmap              * m_pBtmBackGround; ///<背景图片
	CNormalBitmapButtonEx m_BtnOK; ///<确定按钮
	CNormalBitmapButtonEx m_BtnCancel; ///<取消按钮
	CNormalBitmapButtonEx m_BtnQuit; ///<退出按钮


	HBRUSH m_bkBrush;

protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

private:
	CSkinMgr m_skinmgr;

private:
	//换肤
	LRESULT OnExchangeSkin(WPARAM wpara,LPARAM lpara);
};
