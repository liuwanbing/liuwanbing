#pragma once
#include "OpenGL.h"
#include "GLTexture.h"
#include "OpenglEngineHead.h"

class CGLVirtualWindow;

//窗口数组
typedef vector<CGLVirtualWindow *> CGLVirtualWindowPtrArray;			//窗口数组

class OPENGL_ENGINE_CLASS CGLVirtualWindow
{
	friend class CGLVirtualButton;

	//窗口属性
protected:
	bool							m_bActive;							// 激活标志
	bool							m_bEnable;							// 启用标志
	bool							m_bVisible;							// 显示标志

	//属性变量
protected:
	UINT							m_uWindowID;						// 窗口标识
	static UINT						m_uFocusID;							// 焦点标识

	//位置变量
protected:
	CSize							m_WindowSize;						// 窗口大小
	CPoint							m_BenchmarkPos;						// 基准位置

	//内核变量
protected:
	CGLDevice *						m_pOpenGl;							// 设备指针

	//二级窗口
protected:
	CGLVirtualWindow *				m_pParentWindow;					// 上层窗口
	CGLVirtualWindowPtrArray		m_ChildWindowArray;					// 子窗口数组

	//一级窗口
public:
	static CGLVirtualWindowPtrArray	m_WindowArray;						// 窗口数组

public:
	CGLVirtualWindow(void);
	virtual ~CGLVirtualWindow(void);
	
	//窗口标识
public:
	//获取标识
	virtual UINT GetWindowID() { return m_uWindowID; }
	//设置标识
	virtual VOID SetWindowID(UINT uWindowID) { m_uWindowID=uWindowID; }

	//属性对象
public:
	//上层窗口
	virtual CGLVirtualWindow * GetParentWindow() { return m_pParentWindow; }

	//管理函数
public:
	//删除窗口
	virtual VOID DeleteWindow();
	//激活窗口
	virtual bool ActiveWindow(CRect & rcWindow, DWORD dwStyle, UINT uWindowID, CGLDevice* pOpenGl, CGLVirtualWindow * pParentWindow);

	//窗口属性
public:
	//激活属性
	virtual bool IsWindowActive() { return m_bActive; }
	//控制属性
	virtual bool IsWindowEnable() { return m_bEnable; }
	//显示属性
	virtual bool IsWindowVisible() { return m_bVisible; }

	//窗口控制
public:
	//显示窗口
	virtual VOID ShowWindow(bool bVisible);
	//禁止窗口
	virtual VOID EnableWindow(bool bEnable);
	//设置焦点
	virtual VOID SetFocus(bool bFocus = true);

	//窗口位置
public:
	//窗口大小
	virtual VOID GetClientRect(CRect & rcClient);
	//窗口大小
	virtual VOID GetWindowRect(CRect & rcWindow);
	//设置位置
	virtual VOID SetWindowPos(INT nXPos, INT nYPos, INT nWidth, INT nHeight, UINT uFlags);

	//功能函数
public:
	//下属窗口
	virtual bool IsChildWindow(CGLVirtualWindow * pVirtualWindow);

	//系统事件
protected:
	//动画消息
	virtual VOID OnWindowMovie() { return; }
	//创建消息
	virtual VOID OnWindowCreate(CGLDevice * pGLDevice) { return; }
	//销毁消息
	virtual VOID OnWindowDestory() { return; }
	//位置消息
	virtual VOID OnWindowPosition() { return; }

	//重载函数
protected:
	//鼠标事件
	virtual VOID OnEventMouse(UINT uMessage, UINT nFlags, INT nXMousePos, INT nYMousePos)=NULL;
	//按钮事件
	virtual VOID OnEventButton(UINT uButtonID, UINT uMessage, INT nXMousePos, INT nYMousePos)=NULL;
	//绘画窗口
	virtual VOID OnEventDrawWindow(CGLDevice * pGLDevice, INT nXOriginPos, INT nYOriginPos)=NULL;

	//内部函数
private:
	//重置窗口
	virtual VOID ResetWindow();
	//绘画窗口
	virtual VOID OnEventDrawChildWindow(CGLDevice * pGLDevice, INT nXOriginPos, INT nYOriginPos);

	//绘画函数
public:
	//绘画窗口
	static void DraWindows(CGLDevice * pGLDevice);

	//消息处理
public:
	//消息处理
	static GLboolean PreTranslateMessage(MSG* pMsg);
	//消息处理
	static GLboolean DefWindowProc(UINT nMessage, WPARAM wParam, LPARAM lParam);

	//窗口搜索
private:
	//获取窗口
	static CGLVirtualWindow* SwitchToWindow(INT nXMousePos, INT nYMousePos);
	//获取窗口
	static CGLVirtualWindow* SwitchToWindow(CGLVirtualWindow * pVirtualWindow, INT nXMousePos, INT nYMousePos);

};
