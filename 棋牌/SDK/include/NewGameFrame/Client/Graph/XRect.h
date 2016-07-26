#pragma once

#include "CommonDefines.h"
#include <Windows.h>
#include <tchar.h>

class X3DEngine;

/// 画矩形类，利用该类可以在窗口指定位置中绘制矩形
class GRAPH_API XRect
{
	friend class X3DEngine;
public:
	XRect():m_l(0), m_t(0), m_r(10), m_b(10), m_z(0), m_colorBold(0xFF000000), m_colorFill(0xFFFFFFFF), m_style(0), m_thick(0){}
	~XRect();

	/// @defgroup group1 各项属性的设置
	/// 设置矩形的各项属性
	/// @{
	inline void SetWidth(int w){m_r = m_l + w;}		///< 设置宽
	inline void SetHeight(int h){m_b = m_t + h;}	///< 设置高
	inline void SetX(int x){m_r += x-m_l; m_l = x;}	///< 设置左上角横坐标
	inline void SetY(int y){m_b += y-m_t; m_t = y;}	///< 设置左上角纵坐标
	inline void SetThick(int thick = 0){m_thick = thick;}		///< 设置厚度，暂不用
	inline void SetStyle(int style = 0){m_style = style;}		///< 设置风格，非0值为实心矩形
	inline void SetColor(DWORD color){m_colorBold = color;}		///< 设置边框颜色
	inline void SetFillColor(DWORD color){m_colorFill = color;}	///< 设置填充颜色，空心矩形忽略此项
	inline void SetDeep(short z){m_z = z;}						///< 设置深度，离眼睛的距离，越大越远
	/// @} 

	/// 获得位置信息，及当前设置的大小
	inline void GetPosition(WORD &x, WORD &y){x=m_l, y=m_t;}
	inline void GetSize(WORD &w, WORD &h){w=m_r-m_l; h=m_b-m_t;}

	/// ShowHide()显示或隐藏矩形
	void ShowHide(bool bShow = true);

protected:
private:
	int m_l, m_t, m_r, m_b;		///< 位置和尺寸，因桌面窗口大小不可能超过655335，所以用WORD即可
	int m_z;					///< 深度层次
	UINT m_colorBold;			///< 边框颜色
	UINT m_colorFill;			///< 填充颜色
	int m_style;				///< 风格，虚线或实线，待实现20100505
	int m_thick;				///< 线的粗细，待实现20100505
};
