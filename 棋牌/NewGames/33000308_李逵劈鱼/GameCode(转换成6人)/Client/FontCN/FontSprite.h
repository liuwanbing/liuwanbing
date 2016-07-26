//////////////////////////////////////////////////////////////////////////////////////
// 精灵机构
//////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "hgesprite.h"

//////////////////////////////////////////////////////////////////
// 公共定义
#define DELETE_SELF(p)				{ if(p) { delete p; } };
#define DELETE_OBJECT(p)			{ if(p) { delete p; p=NULL; } };
#define DELETE_OBJECTARRAY(p)		{ if(p) { delete [] p; p=NULL; } };
#define RELEASE_OBJECT(p)			{ if(p) { p->Release(); p=NULL; } };

__inline float _floor(float f)
{
	static int _n;
	_asm fld f
	_asm fistp _n
	return (float)_n;
}

//////////////////////////////////////////////////////////////////////////////////////
// 字体精灵
//////////////////////////////////////////////////////////////////////////////////////

class FontSprite
{
public:
	// 获取像素字体指针
	// const char* lpsFontName	- 字体名称
	// int nFaceSize			- 字体大小
	// BOOL bBold				- 是否粗体
	// BOOL bItalic				- 是否斜体
	// BOOL bUnderline			- 是否有下划线
	static FontSprite* Create(const char* lpsFontName, int nFaceSize, BOOL bBold = FALSE, BOOL bItalic = FALSE, BOOL bUnderline = FALSE, BOOL bAntialias = TRUE);

	// 获取适量字体指针
	// const char* lpsTTFile	- 矢量字体路径文件名
	// float nFontSize			- 矢量字体大小
	static FontSprite* CreateEx(const char* lpsTTFile, float nFontSize);

	// 销毁字体
	virtual void	Release() = 0;

	// 渲染文本
	virtual void	Printf( float x, float y, const wchar_t *format, ... ) = 0;
	virtual void	Render( float x, float y, const wchar_t* text ) = 0;
	virtual void	RenderEx( float x, float y, const wchar_t* text, float scale = 1.0f ) = 0;

	// 设置与获取颜色
	virtual void	SetColor( DWORD dwColor, int i = -1 ) = 0;
	virtual DWORD	GetColor( int i = 0 ) = 0;

	// 获取文本区域大小
	virtual SIZE	GetTextSize( const wchar_t* text ) = 0;

	// 根据相对坐标获取字符
	virtual wchar_t	GetCharacterFromPos( const wchar_t* text, float pixel_x, float pixel_y ) = 0;

	// 设置字间距
	virtual void	SetKerningWidth( float kerning ) = 0;
	virtual void	SetKerningHeight( float kerning ) = 0;

	// 获取字间距
	virtual float	GetKerningWidth() = 0;
	virtual float	GetKerningHeight() = 0;

	// 获取字体大小
	virtual float	GetFontSize() = 0;

protected:
	// 方式该抽象类被new
	virtual ~FontSprite(void) { };

};//class FontSprite
