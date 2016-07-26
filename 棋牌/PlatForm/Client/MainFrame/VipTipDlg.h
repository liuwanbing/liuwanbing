#pragma once
#include "afxwin.h"
#include "Resource.h"
#include "SkinMgr.h"
#include "..\UILibrary2003\CconfigDlg.h" 
// CVipTipDlg 对话框

class CVipTipDlg : public CGameFaceGo
{
	DECLARE_DYNAMIC(CVipTipDlg)

public:
	CVipTipDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CVipTipDlg();

// 对话框数据
	enum { IDD = IDD_VIPDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	//立即购买 VIP
	afx_msg void OnBnClickedOk();
	//取消
	afx_msg void OnBnClickedQuxiao();
public:
	//关闭
	afx_msg void OnBnClickedClosebtn();
public:
	 CGameImage  m_backPic;     //背景图
	 CString     m_titelStr;    //标题
	 CRect       m_rectInfoPic; //提示图片显示区域
    CRect       m_rectTitleText;   //标题文字区域

	CNormalBitmapButtonEx m_buyBtn;
	CNormalBitmapButtonEx m_cancelBtn;
	CNormalBitmapButtonEx m_closeBtn;
	CconfigDlg  m_configDlg; //加载底图类. 2012.10.17 yyf
public:
	virtual BOOL OnInitDialog();
public:
	afx_msg void OnPaint();
public:
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

private:
	CSkinMgr m_skinmgr;

	void LoadSkin();
public:
	LRESULT OnExchangeSkin(WPARAM wpara,LPARAM lpara);

	  // 设置窗口圆角 2012.10.15 yyf
	  void SetWndRgn(void);
};
