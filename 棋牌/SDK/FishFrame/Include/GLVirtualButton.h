#pragma once

#include "OpenGL.h"
#include "GLTexture.h"
#include "GLLapseCount.h"
#include "GLVirtualWindow.h"
#include "OpenglEngineHead.h"

class OPENGL_ENGINE_CLASS CGLVirtualButton : public CGLVirtualWindow
{
	//状态变量
protected:
	bool							m_bMouseDown;						//按下标志
	bool							m_bMouseMove;						//经过标志
	bool							m_bButtonFocus;						//焦点标示				
	CRect							m_rcButtonRect;						//按钮区域

	//资源变量
protected:
	LPCTSTR							m_pszTypeName;						//资源类型
	LPCTSTR							m_pszResource;						//资源信息
	HINSTANCE						m_hResInstance;						//资源句柄

	//动画变量
protected:
	WORD							m_wImageIndex;						//过渡索引
	CGLLapseCount					m_ImageLapseCount;					//流逝计数

	//资源变量
protected:
	CGLTexture						m_GLTextureButton;					//按钮纹理

public:
	CGLVirtualButton(void);
	~CGLVirtualButton(void);


	//系统事件
protected:
	//动画消息
	virtual VOID OnWindowMovie();
	//创建消息
	virtual VOID OnWindowCreate(CGLDevice * pGLDevice);
	//销毁消息
	virtual VOID OnWindowDestory();

	//重载函数
protected:
	//鼠标事件
	virtual VOID OnEventMouse(UINT uMessage, UINT nFlags, INT nXMousePos, INT nYMousePos);
	//按钮事件
	virtual VOID OnEventButton(UINT uButtonID, UINT uMessage, INT nXMousePos, INT nYMousePos);
	//绘画窗口
	virtual VOID OnEventDrawWindow(CGLDevice * pGLDevice, INT nXOriginPos, INT nYOriginPos);

	//功能函数
public:
	//设置焦点
	VOID SetButtonFocus(bool bButtonFocus) { m_bButtonFocus = bButtonFocus; }
	//设置区域
	VOID SetButtonRect(CRect rcButtonRect);
	//加载位图
	VOID SetButtonImage(CGLDevice * pGLDevice, HINSTANCE hResInstance, LPCTSTR pszResource, LPCTSTR pszTypeName,  DWORD wImageType);

	//内部函数
protected:
	//调整控件
	VOID RectifyControl(CGLDevice * pGLDevice);
};
