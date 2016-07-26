#pragma once

#include "CommonDefines.h"
#include <Windows.h>
#include <set>
#pragma warning(disable:4251)

class X2DFont;
class XFont_Greater;

/// 用来显示文字的接口类
class GRAPH_API XFont
{
	friend class X2DFont;
	friend class XFont_Greater;
public:
	XFont(void);
	~XFont(void);

	/// Init() 初始化字体
	/// @param[in] szFontName 字体名称，须与windows字体一致，如找不到，则默认为宋体
	/// @param[in] nSize 字体大小
	/// @param[in] bItalic 是否斜体
	/// @param[in] bBold 是否加粗
	/// @param[in] bUnderline 是否加下划线
	/// @param[in] bIsManyChar 是否可能会有很多不同汉字
	bool Init( TCHAR *szFontName, int nSize, bool bItalic = false, bool bBold = false, bool bUnderline = false, bool bIsManyChar =false );
	/// SetString()更改字符串内容
	bool SetString(TCHAR *szText);
	/// SetXY()修改文本在窗口中的坐标
	void SetXY(short x, short y);
	/// SetDeep()设置文本的深度
	void SetDeep(short z);
	/// SetColor()设置文本的颜色
	/// param 前四个参数对应左上，右上，左下，右下颜色值，中间部分按四角插值，
	/// 第五个参数表示字符被选择后表现出的颜色，只有一种
	void SetColor(DWORD colorLT, DWORD colorRT,DWORD colorLB,DWORD colorRB, DWORD colorSelect = 0xFFFFFFFF);
	/// ShowHide()显示或隐藏文字
	void ShowHide(bool bShow = true);
	/// CommitChanges()提交所作的更改
	void CommitChanges();
	/// 移动位置
	/// @param[in] dir 方向代码 0表示x方向，1表示y方向，2表示z方向
	/// @param[in] offset 偏移量，可为正负
	void Move(int dir, int offset)
	{
		switch(dir)
		{
		case 0: m_x += offset; break;
		case 1: m_y += offset; break;
		case 2: m_z += offset; break;
		default: break;
		}
	}
	/// 选择文字
	/// @param st 选择的开始位置，第几个字符。0为第一个字符
	/// @param ed 选择的结束位置，第几个字符。0为第一个字符
	/// 结束选择时，调用SelectText(-1, -1);
	void SelectText(short st, short ed){m_selBegin = st; m_selEnd = ed;}
	/// 设置裁剪范围
	void SetClipBox(short l, short r, short t, short b){m_sClipBox[0]=l; m_sClipBox[1]=r;m_sClipBox[2]=t; m_sClipBox[3]=b;}

private:
	//Tstring m_str;			///< 本控件的字符串
	TCHAR *m_szText;		///< 本控件的字符串
	int	m_nLen;				///< 本控件的字符数组长度
	X2DFont *m_p2DFont;		///< 引擎字体指针
	short m_x, m_y;			///< 字体位置
	short m_z;				///< 字体深度，值越大离眼睛越远，在窗口中越处于底层，越容易被其它元素遮挡
	DWORD m_color[4];		///< 字体颜色，0-3对应于字体所属矩形的左上，右上，左下，右下角。
	DWORD m_colorSelect;	///< 反选颜色，只有一种，不提供不同顶点不同颜色
	short m_selBegin;		///< 从第几个字符开始选择
	short m_selEnd;			///< 到第几个字符结束选择
	short m_sClipBox[4];		///< 裁剪范围，0,1,2,3对应左，右，上，下
};

/// XFont指针排序规则
class XFont_Greater : public std::binary_function<XFont *, XFont *, bool>
{
public:
	bool operator()(const XFont *pl, const XFont *pr)
	{
		return (pl->m_z > pr->m_z);
	}
};

/// 使用方法
