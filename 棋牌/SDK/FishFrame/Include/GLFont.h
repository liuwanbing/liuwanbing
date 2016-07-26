#pragma once

#include "OpenGL.h"
#include "OpenglEngineHead.h"

struct mapTextInfo
{
	UCHAR*	pBits;					//BITMAP指针
	INT		nWidtht;				//文字宽度
	INT		nHeight;				//文字高度
	DWORD	dwRetentionTime;		//停留时间

	mapTextInfo( UCHAR* pParBits, INT nParWidtht, INT nParHeight )
	{
		pBits = pParBits;
		nWidtht = nParWidtht;
		nHeight = nParHeight;
		dwRetentionTime = timeGetTime();
	}
};
typedef map< CString , mapTextInfo >			mapText;


class OPENGL_ENGINE_CLASS CGLFont  
{

protected:
	HFONT							m_hFont;				// 字体
	HFONT							m_hOldFont;				// 旧字体
	HDC								m_Hdc;					// 窗口句柄
	mapText							m_mapText;				// 输出文字


	// 
public:
	CGLFont();
	virtual ~CGLFont();

	// 创建函数
public:
	// 删除字体
	bool DeleteFont();
	// 删除缓存
	bool DeleteCache();
	// 创建字体
	bool CreateFont(int nHeight, int nWeight, LPCTSTR lpszFaceName);

	// 输出字体
public:
	// 输出字体
	bool DrawText(CGLDevice* pOpenGL, LPCTSTR pszString, CRect rcDraw, UINT uFormat, COLORREF GLColor);
	// 输出文字
	bool TextOut(CGLDevice* pOpenGL, LPCTSTR pszString, int nPosX, int nPosY, COLORREF GLColor);

	// 获取文字大小
	CSize GetTextSize(LPCTSTR pszString);


};

