#ifndef GAME_FRAME_VIEW_GL_HEAD_FILE
#define GAME_FRAME_VIEW_GL_HEAD_FILE

#pragma once

#include "GameFrameHead.h"
#include "GameFrameView.h"

//////////////////////////////////////////////////////////////////////////////////

//视图框架
class GAME_FRAME_CLASS CGameFrameViewGL : public CGameFrameView
{
	//状态变量
protected:
	bool							m_bInitGL;							//创建标志
	HANDLE 							m_hRenderThread;					//渲染线程

	//辅助变量
protected:
	CRect							m_RectDirtySurface;					//重画区域

	//帧数统计
protected:
	DWORD							m_dwDrawBenchmark;					//基准起点
	DWORD							m_dwDrawLastCount;					//绘画次数
	DWORD							m_dwDrawCurrentCount;				//绘画次数

	//组件变量
public:
	CGLFont							m_GLFont;
	CGLDevice						m_GLDevice;

	//函数定义
public:
	//构造函数
	CGameFrameViewGL();
	//析构函数
	virtual ~CGameFrameViewGL();

	//重载函数
public:
	//重置界面
	virtual VOID ResetGameView();
	//调整控件
	virtual VOID RectifyControl(INT nWidth, INT nHeight);
	//界面更新
	virtual VOID InvalidGameView(INT nXPos, INT nYPos, INT nWidth, INT nHeight);

	//界面函数
private:
	//动画驱动
	virtual VOID CartoonMovie()=NULL;
	//配置界面
	virtual VOID InitGameView(CGLDevice* pGLDevice, INT nWidth, INT nHeight)=NULL;
	//绘画界面
	virtual VOID DrawGameView(CGLDevice* pGLDevice, INT nWidth, INT nHeight)=NULL;

	//重载函数
protected:
	//消息解释
	virtual BOOL PreTranslateMessage(MSG * pMsg);
	//默认回调
	virtual LRESULT DefWindowProc(UINT nMessage, WPARAM wParam, LPARAM lParam);

	//功能函数
public:
	//渲染线程
	VOID StartRenderThread();

	//输出字体
public:
	//输出字体
	VOID DrawTextString(CGLDevice* pGLDevice, LPCTSTR pszString, CRect rcDraw,UINT nFormat, COLORREF GLColor);
	//输出字体
	VOID DrawTextString(CGLDevice* pGLDevice, LPCTSTR pszString, INT nXPos, INT nYPos, COLORREF GLColor);
	//绘画字符
	VOID DrawTextString(CGLDevice* pGLDevice, LPCTSTR pszString, CRect rcDraw, UINT nFormat, COLORREF crText, COLORREF crFrame);
	//绘画字符
	VOID DrawTextString(CGLDevice* pGLDevice, LPCTSTR pszString, INT nXPos, INT nYPos,  COLORREF crText, COLORREF crFrame);

	//消息映射
protected:
	//绘画函数
	VOID OnPaint();
	//位置消息
	VOID OnSize(UINT nType, INT cx, INT cy);

	//自定消息
protected:
	//数据消息
	LRESULT OnMessageGLStatus(WPARAM wParam, LPARAM lParam);
	//渲染消息
	LRESULT OnMessageGLRender(WPARAM wParam, LPARAM lParam);

	//静态函数
private:
	//渲染线程
	static VOID GLRenderThread(LPVOID pThreadData);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnTimer(UINT nIDEvent);
};

//////////////////////////////////////////////////////////////////////////////////

#endif