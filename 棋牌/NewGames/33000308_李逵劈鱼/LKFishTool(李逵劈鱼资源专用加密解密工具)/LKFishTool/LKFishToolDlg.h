
// LKFishToolDlg.h : 头文件
//

#pragma once


// CLKFishToolDlg 对话框
class CLKFishToolDlg : public CDialogEx
{
// 构造
public:
	CLKFishToolDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_LKFISHTOOL_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();


private:
	bool	EncryptFishFile(const CString& strFile, BOOL bDeleteSourceFile);
	bool	DecryptFishFile(const CString& strFile, BOOL bDeleteSourceFile);
private:
	CString m_strSource;

public:
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton4();
};
