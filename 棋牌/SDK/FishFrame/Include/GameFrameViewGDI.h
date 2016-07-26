#ifndef GAME_FRAME_VIEW_GDI_HEAD_FILE
#define GAME_FRAME_VIEW_GDI_HEAD_FILE

#pragma once

#include "GameFrameView.h"
#include "GameFrameHead.h"

//////////////////////////////////////////////////////////////////////////////////

//视图框架
class GAME_FRAME_CLASS CGameFrameViewGDI : public CGameFrameView
{
	//缓冲变量
private:
	CSize							m_SizeBackSurface;					//表面大小
	CRect							m_RectDirtySurface;					//重画区域
	CImage							m_ImageBackSurface;					//缓冲表面
	CSkinButton						m_btCloseAwards;					//关闭按钮

	//游戏元素
protected:
	CPngImageEx						m_ImageUserReadyEx;					//用户准备
	CPngImageEx                     m_ImageClockItemEx;
	CPngImageEx                     m_ImageClockBackEx;

	//函数定义
public:
	//构造函数
	CGameFrameViewGDI();
	//析构函数
	virtual ~CGameFrameViewGDI();

	//重载函数
private:
	//绘画界面
	virtual VOID DrawGameView(CDC * pDC, INT nWidth, INT nHeight)=NULL;

	//重载函数
public:
	//重置界面
	virtual VOID ResetGameView();
	//调整控件
	virtual VOID RectifyControl(INT nWidth, INT nHeight);
	//界面更新
	virtual VOID InvalidGameView(INT nXPos, INT nYPos, INT nWidth, INT nHeight);
	//桌面消息
	virtual void AddGameTableMessage(LPCTSTR pszMessage,WORD wLen,WORD wType);

	//游戏元素
public:
	//绘画准备
	VOID DrawUserReady(CDC * pDC, INT nXPos, INT nYPos);
	//绘画时间
	VOID DrawUserClock(CDC * pDC, INT nXPos, INT nYPos, WORD wUserClock);
	//绘画头像
	VOID DrawUserAvatar(CDC * pDC, INT nXPos, INT nYPos, IClientUserItem * pIClientUserItem);
	//绘画头像
	VOID DrawUserAvatar(CDC * pDC, INT nXPos, INT nYPos, INT nWidth, INT nHeight, IClientUserItem * pIClientUserItem);

	//画图函数
public:
	//绘画背景
	VOID DrawViewImage(CDC * pDC, CBitImage & BitImage, BYTE cbDrawMode);
	//绘画字符
	VOID DrawTextString(CDC * pDC, LPCTSTR pszString, COLORREF crText, COLORREF crFrame, CRect rcRect,CDFontEx *pFontEx=NULL);
	//绘画字符
	VOID DrawTextString(CDC * pDC, LPCTSTR pszString, COLORREF crText, COLORREF crFrame, INT nXPos, INT nYPos,CDFontEx *pFontEx=NULL);
	//绘画数字
	VOID DrawNumberString(CDC * pDC, CPngImage &PngNumber, LONG lNumber, INT nXPos, INT nYPos,BYTE cbAlign=TA_LEFT);
	//比赛信息
	VOID DrawMatchInfo(CDC * pDC);

	//消息映射
protected:
	//绘画函数
	VOID OnPaint();
	//关闭奖状
	VOID OnBnClickedCloseAwards();

	DECLARE_MESSAGE_MAP()

	//消息映射
public:
	afx_msg void OnTimer(UINT nIDEvent);

	//WIN7支持
public:
	//是否特殊绘制
	virtual bool RealizeWIN7() { return false; }
	//绘画背景
	VOID DrawViewImage(CDC * pDC, CBitImageEx & BitImage, BYTE cbDrawMode);
	//绘画数字
	VOID DrawNumberString(CDC * pDC, CPngImageEx &PngNumber, LONG lNumber, INT nXPos, INT nYPos,BYTE cbAlign=TA_LEFT);
	//写字符
	VOID DrawText(CDC * pDC, LPCTSTR lpszString, int nCount, LPRECT lpRect, UINT nFormat, CDFontEx* pFontEx = NULL);
	//写字符
	VOID DrawText(CDC * pDC, const CString& str, LPRECT lpRect, UINT nFormat, CDFontEx* pFontEx = NULL);
	//写字符
	VOID TextOut(CDC * pDC, int x, int y, LPCTSTR lpszString, int nCount, CDFontEx* pFontEx = NULL);
	//写字符
	VOID TextOut(CDC * pDC, int x, int y, const CString& str, CDFontEx* pFontEx = NULL);
};

//////////////////////////////////////////////////////////////////////////////////

#endif