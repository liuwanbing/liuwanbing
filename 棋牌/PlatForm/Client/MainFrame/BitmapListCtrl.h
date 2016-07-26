#ifndef BITMAPLISTCTRL_HEAD_H
#define BITMAPLISTCTRL_HEAD_H

#if !defined(AFX_LISTCTRLHIDDENSB_H__16168A9D_2FBD_4689_84BD_2EB4509C255E__INCLUDED_)
#define AFX_LISTCTRLHIDDENSB_H__16168A9D_2FBD_4689_84BD_2EB4509C255E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ListCtrlHiddenSB.h : header file
//

#define LCSB_CLIENTDATA 1
#define LCSB_NCOVERRIDE 2

#pragma once
#include "userlistctrl.h"

class CBitmapListCtrl :
	public CUserListCtrl
{
public:
	CBitmapListCtrl(void);
public:
	~CBitmapListCtrl(void);
public:
	CGameImage m_itembk;
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	//³õÊ¼»¯
	virtual void InitListCtrl(GetOrderName * pOrderFunction, UINT uComType);
	TCHAR m_sztitle[100];
	void HideScrollBars(int Type, int Which=SB_BOTH);
	CRect  m_rlog,m_rname,m_rorder,m_rcoin,m_rcoinc,m_rloss,m_rwin,m_rdesk;
	int			m_uComType;
protected:
	//{{AFX_MSG(CListCtrlHiddenSB)
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp);
	//}}AFX_MSG
	//±³¾°º¯Êý
	afx_msg BOOL OnEraseBkgnd(CDC * pDC);

	DECLARE_MESSAGE_MAP()
private:
	BOOL NCOverride;
	int Who;
};
#endif

#endif