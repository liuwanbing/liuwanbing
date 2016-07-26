#pragma once


// CUserDetailsDlg 对话框

class CUserDetailsDlg : public CDialog
{
	DECLARE_DYNAMIC(CUserDetailsDlg)

public:
	CUserDetailsDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CUserDetailsDlg();

// 对话框数据
	//enum { IDD = IDD_USER_DETAILS };

private:
	CNormalBitmapButtonEx  m_btClose;
	CGameImage  m_BackBitmap;
	UserItemStruct * m_pUserItem;
	WCHAR		m_wszBuffer[MAX_PATH];			// 宽字符数组（绘画png）
	COLORREF    m_clrLabel, m_clrText;
	CString     m_strMoneyType;                 // 货币类型
	CString     m_strCombatGainsFormat;         // 战绩输出格式
	TCHAR       szFileName[MAX_PATH];
	int			m_isShow;                       //是否显示战绩
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

public:
	void SetUserItem(UserItemStruct * pUserItem);
	void OnCancel();

public:
	UINT m_uGameNameID;
	UINT m_uComType;

	CSkinMgr	m_skinmgr;

public:
	void LoadSkin();
};
