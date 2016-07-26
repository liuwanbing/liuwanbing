#pragma once

#include "resource.h"
#include "afxcmn.h"
// CResult 对话框

class CResult : public CDialog
{
	DECLARE_DYNAMIC(CResult)

public:
	CResult(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CResult();

	CGameImage m_img;	//背景图片

// 对话框数据
	enum { IDD = IDD_RESULT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	virtual BOOL OnInitDialog();

	//数据清零
	void ClearAll();

public:
	int		m_iNtStation;//庄家位置
	int		m_iMeStation;//自己的位置
	__int64     m_iNtWinMoney;//庄赢的钱
	__int64		m_iXianMoney;//闲赢的钱
	__int64     m_iMeWinMoney;//自己赢的钱
	int     m_iPaiDian[2];  //庄闲的牌点
	int     m_iZPaiXing[2]; //庄家的牌型
	int     m_iXPaiXing[2]; //闲家的牌型


};
