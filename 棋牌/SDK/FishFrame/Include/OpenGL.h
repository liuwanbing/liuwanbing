#pragma once

#include <mmsystem.h>
#include "OpenglEngineHead.h"

struct stuThreadRC
{
	HDC		pHdc;
	HGLRC	pHGLRc;
};


class OPENGL_ENGINE_CLASS CGLDevice
{

	// 变量定义
private:
	HWND					m_hWnd;				// 窗口句柄
	CDC*					m_pDC;				// 显示绘画DC
	HGLRC					m_hRC;				// OpenGL渲染描述表句柄
	CSize					m_WinSize;			// 窗口大小
	vector<stuThreadRC>		m_ThreadRC;			// 线程RC

	// 静态定义
private:
	// 鼠标信息
	static PVOID			m_pTextureMuose;	// 鼠标指针	
	static CPoint			m_ptMouse;			// 鼠标位置
	static CSize			m_sizeMouseOffset;	// 鼠标偏移

	//构造析构
public:
	CGLDevice(void);
	~CGLDevice(void);

	// 获取函数
public:
	// 获取窗口
	const HWND				GetHWND() { return m_hWnd; }
	// 获取DC
	const CDC*				GetDC()   { return m_pDC; }
	// 获取RC
	const HGLRC				GetGLRC() { return m_hRC; };
	// 获取窗口大小
	const CSize*			GetSize() { return &m_WinSize; };

	// 鼠标函数
public:
	// 绘画鼠标
	inline static GLvoid	SetCursor( PVOID pTextureMuose ) { m_pTextureMuose = pTextureMuose; };
	// 绘画鼠标位置
	inline static GLvoid	SetCursorPos( CPoint ptMouse ) { m_ptMouse = ptMouse; };
	// 绘画鼠标偏移
	inline static GLvoid	SetCursorOffset( CSize sizeMouseOffset ) { m_sizeMouseOffset = sizeMouseOffset; };
	// 获取鼠标
	inline static PVOID		GetCursor() { return m_pTextureMuose; };

public:
	// 创建函数
	GLboolean				CreateOpenGL(CWnd* pWnd);
	// 创建新的RC(用于多线程)
	GLboolean				CreateThreadRC( int& nIndex );
	// 设置线程RC
	GLboolean				SetThreadRC( int nIndex , bool bNULL);
	// 销毁线程RC
	GLboolean				KillThreadRC( int nIndex );
	// 正常销毁窗口
	GLvoid					KillGLWindow();		
	// 窗口改变
	GLvoid					OnSize(int cx, int cy);
	// 绘画开始
	GLvoid					GLBegin(CGLDevice* pGLDevice);
	// 绘画结束
	GLvoid					GLEnd(CGLDevice* pGLDevice);

	// 绘画函数
public:
	// 绘画点
	GLvoid					DrawPoint(int nX, int nY, COLORREF rgb);
	// 绘画线
	GLvoid					DrawLine(int nBeginX, int nBeginY, int nEndX, int nEndY, COLORREF rgb);
	// 绘画粗糙圆
	GLvoid					DrawRoughRound( int nRoundDotX, int nRoundDotY, int nRadius, COLORREF rgb );
	// 绘画精细圆
	GLvoid					DrawFineRound( int nRoundDotX, int nRoundDotY, int nRadius, COLORREF rgb );
	// 绘画多边形
	GLvoid					DrawPolygon( CPoint* pPoint, int nPointCount, COLORREF rgb );
	// 绘画多边框
	GLvoid					DrawMoreBorder( CPoint* pPoint, int nPointCount, COLORREF rgb );
	// 绘画矩形
	GLvoid					DrawRect( CRect rect, COLORREF rgb );
};
