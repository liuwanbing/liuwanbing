#pragma once
#include "afxwin.h"
#include "StdAfx.h"
#include "resource.h"
#include <list>
#include "SkinMgr.h"
#include "afxcmn.h"
#include "..\UILibrary2003\CconfigDlg.h" 
using namespace std;
/********************************************************************
	created:	2009/03/04
	created:	4:3:2009   16:51
	filename: 	客户端组件\系统主框架\AppealDlg.h
	file path:	客户端组件\系统主框架
	file base:	AppealDlg
	file ext:	h
	author:		yjj
	purpose:	投诉框的界面设计

    Copyright szbzw 2009 - All Rights Reserved

*********************************************************************/
// CAppealDlg 对话框

//yjj 090304 add this dlg for appeal

//定义ftp上传函数
typedef void (WINAPI *PFCALLBACK)(CString sFileName,long nFileSize);
__declspec(dllimport) int DownloadFile(CStringArray *csa,HWND pWnd,UINT message);
__declspec(dllimport) int UploadFile(CStringArray *csa,HWND pWnd,UINT message);
__declspec(dllimport) void FTPCancel();
__declspec(dllimport) void WINAPI GetServerList(CStringArray *csa,PFCALLBACK Func);


struct RecordFileStruct 
{
	CString strWholeName;   //游戏的录像文件的完整名字
	CString strFileName;
	CString strGameName;    //游戏名称
	//wushuqun 2009.5.20
	//完整路径
	CString strGamePath;

	CString strGameTime;    //游戏时间
	CTime   timeCreatTime;  //录像文件的创建时间

	//是为了让list 自动按降序排列，才如此定义
	bool operator>(RecordFileStruct& comp)
	{
		if (timeCreatTime >= comp.timeCreatTime)
		{
			return false;
		}
		return true;
	}
	bool operator<(RecordFileStruct& comp)
	{
		if (timeCreatTime <= comp.timeCreatTime)
		{
			return false;
		}
		return true;
	}
	bool operator==(RecordFileStruct& comp)
	{
		if (timeCreatTime != comp.timeCreatTime)
		{
			return false;
		}
		return true;
	}
};
class CAppealDlg : public CDialog
{
	DECLARE_DYNAMIC(CAppealDlg)

public:
	explicit CAppealDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CAppealDlg();

// 对话框数据
	enum { IDD = IDD_DLG_APPEAL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();

	//通过改选项，向bcf log目录中保存一个bcf文件
	static void WriteGameInfoFile(TCHAR* pstGameName,UINT uNameId);

	void OpenAppealDlg(TCHAR* pUserName);

protected:
	//把录像文件按时间逆序，然后反应到控件上  yjj 090304
	void ProcessRecordFile(const CString& strUserName);
	//需要一个文件，使得id和游戏名一一对应
	CString GetGameName(DWORD dwHandle,const CString& uGameId);

	//根据文件名得到nameid
	CString GetFileNameID(const CString& strFileName);
	//根据文件名，得到文件创建时间
	//CString GetFileTime(const CString& strFileName);



	
private:

	CNormalBitmapButtonEx m_btnCancle;
	CNormalBitmapButtonEx m_btnOk;
	CGameImage            m_imgDlgBk;
	CNormalBitmapButtonEx m_bnClose;


	CString   m_strFtpAddress;
	CString   m_strFtpUserName;
	CString   m_strFtpPwd;


    list<RecordFileStruct> m_fileList;

	CString m_strUserName;//玩家用户名

	CListCtrl m_FileListCtrl;

	CString m_strInfoFileWholeName;
	CString m_strInfoFileName;

	//用户列表
	//CListBox m_ListBox;
	afx_msg void OnPaint();
	
	bool m_bUpLoad;
protected:
	virtual void OnOK();
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
public:
	afx_msg void OnBnClickedClose();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnNMDblclkList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

public:
	LRESULT OnExchangeSkin(WPARAM wpara,LPARAM lpara);

private:
	void Init();
	//换肤
	void LoadSkin();
private:
	CSkinMgr m_skinmgr;
	CconfigDlg  m_configDlg; //加载底图类. 2012.10.15 yyf
public:
	  // 设置窗口圆角 2012.10.15 yyf
	  void SetWndRgn(void);
};
