#pragma once

#include "CommonDefines.h"
#include <Windows.h>
#include <tchar.h>

class X3DEngine;

/// 画矩形类，利用该类可以在窗口指定位置中绘制直线
class GRAPH_API XLine
{
	friend class X3DEngine;
public:
	XLine():m_x1(0), m_y1(0), m_x2(100), m_y2(100), m_z(0), m_color1(0xFF000000), m_color2(0xFF000000), m_style(0), m_thick(0){}
	~XLine();

	/// @defgroup group1 各项属性的设置
	/// 设置直线的各项属性
	/// @{
	inline void SetPos(int x1, int y1, int x2, int y2){ m_x1 = x1; m_x2 = x2; m_y1 = y1; m_y2=y2; }		///< 设置位置
	inline void SetDeep(short z){m_z = z;}						///< 设置深度，离眼睛的距离，越大越远
	inline void SetColor(UINT color1, UINT color2){m_color1 = color1; m_color2 = color2;}					///< 设置颜色
	/// @} 

	/// ShowHide()显示或隐藏
	void ShowHide(bool bShow = true);

protected:
private:
	int m_x1, m_y1, m_x2, m_y2;		///< 起点终点位置
	int m_z;					///< 深度层次
	UINT m_color1;			///< 起点颜色
	UINT m_color2;			///< 终点颜色
	int m_style;				///< 风格，虚线或实线，待实现20100505
	int m_thick;				///< 线的粗细，待实现20100505
};
