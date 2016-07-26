#pragma once
#ifndef D_FONT_EX_HEAD_FILE
#define D_FONT_EX_HEAD_FILE

#pragma once

#include "WHImageHead.h"

class WH_IMAGE_CLASS CDFontEx
{

	//变量定义
public:
	CWnd*								m_pWnd;
	CFont								m_GDIFont;
	void*								m_pD2DFont;
	int									m_nWidth;


public:
	CDFontEx(void);
	~CDFontEx(void);

public:
	//创建字体
	void CreateFont(CWnd* pWnd, LPCTSTR strInfo, int nWidth, int nWeight);
	//获取字符长度
	static int GetCharLength(TCHAR chInfo, int nWidth);

public:
	//绘画字体
	void DrawText(CDC* pDC, LPCTSTR str, LPRECT lpRect, COLORREF colorref, UINT nFormat);
	//绘画字体
	void DrawText(CDC* pDC, LPCTSTR str, int XPos, int YPos, COLORREF colorref, UINT nFormat);

public:
	//绘画字体
	static void DrawText(CWnd* pWnd, CDC* pDC, int nWidth, int nWeight, LPCTSTR str, LPRECT lpRect, COLORREF colorref, UINT nFormat);
	//绘画字体
	static void DrawText(CWnd* pWnd, CDC* pDC, int nWidth, int nWeight, LPCTSTR str, int XPos, int YPos, COLORREF colorref, UINT nFormat);

public:
	//删除字体
	void DeleteFont();
};
#endif