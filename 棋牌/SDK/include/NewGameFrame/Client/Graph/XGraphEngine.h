#ifndef __XGraphEngine_H
#define __XGraphEngine_H

#include "CommonDefines.h"
#include <windows.h>
#include <tchar.h>

/// 提供给框架调用的接口，只用来初始化，具体的实现不需要导出

/// 图像引擎接口
class XGraphEngine
{
public:
	/// 虚拟析构函数，以便释放资源
	virtual ~XGraphEngine(){}
	/// 初始化，提供窗口句柄给D3D设备
	virtual HRESULT Initial(HWND hwnd, HANDLE hKeyBoard, HANDLE hMouse) = 0;
	/// 开始绘制
	virtual HRESULT Start2Draw() = 0;
	/// 结束绘制
	virtual HRESULT FinishDraw() = 0;
	/// 窗口恢复活动状态
	virtual bool OnActivate(bool bIsActived)=0;
	/// 设备丢失
	virtual void OnDeviceLost()=0;
	/// 设备恢复
	virtual HRESULT OnResetDevice()=0;
protected:
	/// 获取执行文件所在目录完整路径
	const TCHAR *GetAppPath()
	{
		static TCHAR s_Path[MAX_PATH];
		static bool s_bIsReady = false;
		if (!s_bIsReady)
		{
			ZeroMemory(s_Path,sizeof(s_Path));
			DWORD dwLength=GetModuleFileName(GetModuleHandle(NULL), s_Path, sizeof(s_Path));
			TCHAR *p = _tcsrchr(s_Path, TEXT('\\'));
			++p;
			*p = TEXT('\0');
			s_bIsReady = true;
		}
		return s_Path;
	}
};
extern GRAPH_API XGraphEngine *GlbGraphEngine();
#endif//__XGraphEngine_H