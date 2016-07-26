#pragma once
#ifndef SHOWSCORE_HEAD_H
#define SHOWSCORE_HEAD_H
#include "PublicDefine.h"

#include"AFCImage.h"
#include"ClientComStruct.h"
#include "SkinMgr.h"
// CShowScore 对话框
#define		IDR_NONE	0
#define		IDR_DOWN	1
#define		IDR_DELAY	2
#define		IDR_UP		3
// 积分榜类
class EXT_CLASS CShowScore : public CDialog
{
	DECLARE_DYNAMIC(CShowScore)

public:
	CShowScore(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CShowScore();

// 对话框数据
//	enum { IDD = IDD_SHOWSCORE };
public:
	int		m_listx,m_listy;	// 列表开始的位置
	int		m_showh;			// 隐藏显示时隐藏的高度
	int     m_itemh,m_w1,m_w2,m_w3,m_w4,m_w5;	// 列表项高度
	int		m_count,w,h;		// 列表行数,高和宽的间隔

	int		m_iShowType;

	UserItemStruct					* m_pUserItem[MAX_PEOPLE];	// 用户信息指针
	__int64		m_allscore[MAX_PEOPLE];		// 总分
	__int64		m_lastscore[MAX_PEOPLE];	// 上次分
	__int64     m_allmoney[MAX_PEOPLE];		// 总金币
	__int64     m_lastmoney[MAX_PEOPLE];	// 上次金币
	int     m_ucomtype;					// 游戏类型
	COLORREF	m_textcr;				// 颜色
protected:
	bool	m_havemouse;				// 鼠标是否在窗口上
	int		m_curx,cury;				// 坐标
	int		m_state;					// 当前状态
protected:
	CGameImage	m_bk;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
public:
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
public:
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	
	afx_msg LRESULT OnMouseLeave(WPARAM w, LPARAM l);
public:
	virtual BOOL OnInitDialog();
protected:
	virtual void OnCancel();
	virtual void OnOK();
public:
	void SetBkImage(TCHAR  path[]);
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
public:
	void UpdateUser(int station, UserItemStruct * pUserItem);
public:
	void SetUserScore(int station,__int64 fen, __int64 money=0);
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
public:
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
public:
	void SetTextColor(COLORREF color);
public:
	void Show(void);
public:
	void Delay(void);
public:
	void Hide(void);

	bool m_bShow;
	void ShowWnd();
	void HideWnd();
public:
	void End(void);
	//
	void ClearUser(void);
	//
	void ClearScore(void);

	BOOL SetWindowPos(const CWnd* pWndInsertAfter, int x, int y,
		int cx, int cy, UINT nFlags);
private:
	CSkinMgr	m_skinmgr;
};
#endif