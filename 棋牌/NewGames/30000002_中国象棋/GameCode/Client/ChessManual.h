#pragma once
///#include "e:\20050611_pmbgame\newgame\客户端组件\界面库.2003\afcdialog.h"

#include "Stdafx.h"
#include "Resource.h"

class CChessManual :
	public CGameFaceGo
{
public:
	CChessManual(void);
	~CChessManual(void);

public:
	CButton				m_btOk;							///< 确定按钮
	CButton				m_btCancel;						///< 取消按钮
	enum					{ IDD = IDD_DLG_QI_PU };	///< 对话框数据
	CWnd *m_pParnet;

public:
	void SetQiPu(int iStartI, int iStartJ, int iEndI, int iEndJ, int iQiZi, int type, int iMeFlag);	///< 设置棋谱
	void ClearAll();
	void UpdateInfo();
private:
	void GetCharSet(int iOrder ,CString &str);

	int m_iBuCount;		///< 走步记数
	int m_iFlag;
	CString m_szString;

public:
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);
public:
	CString m_strQiPu;
};

