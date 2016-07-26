////////////////////////////////////////////////////////////////////////////
// 使用 FreeType2 库实现汉字显示。
// 微妙的平衡
// 2007-10-12
////////////////////////////////////////////////////////////////////////////

#ifndef TYPEFONT_H
#define TYPEFONT_H

#include "FontSprite.h"
#include <vector>

// 包含矢量字体支持的lib
//#pragma comment(lib,"FontCN/freetype/objs/freetype235MT.lib")

extern "C"
{
#include "ft2build.h"
#include "freetype\freetype.h"
};

////////////////////////////////////////////////////////////////////////////
// 字体轮廓
typedef struct tagTTFontGlyph
{
	float		x;
	float		y;
	float		w;
	float		h;
	HTEXTURE	hTexture;
	void Reset()
	{
		x=0;y=0;w=0;h=0;hTexture=NULL;
	}
	void Release(HGE* pHGE)
	{
		if (hTexture)
		{
			pHGE->Texture_Free( hTexture );
			hTexture = NULL;
		}
	};
}TTFONTGLYPH;

////////////////////////////////////////////////////////////////////////////
// 字体
class TypeFont :
	public FontSprite
{
public:
	TypeFont();
	virtual ~TypeFont();

	// 释放一个TypeFont精灵对象
	virtual void	Release();

	// 渲染文本
	virtual void	Printf( float x, float y, const wchar_t *format, ... );
	virtual void	Render( float x, float y, const wchar_t* text );
	virtual void	RenderEx( float x, float y, const wchar_t* text, float scale = 1.0f );

	// 设置与获取颜色
	virtual void	SetColor( DWORD dwColor, int i = -1 );
	virtual DWORD	GetColor( int i = 0 );

	// 获取文本宽高
	virtual SIZE	GetTextSize( const wchar_t* text );

	// 根据坐标获取字符
	virtual wchar_t	GetCharacterFromPos( const wchar_t* text, float pixel_x, float pixel_y );

	// 设置字间距
	virtual void	SetKerningWidth( float kerning );
	virtual void	SetKerningHeight( float kerning );

	// 获取字间距
	virtual float	GetKerningWidth();
	virtual float	GetKerningHeight();

	// 字体大小
	virtual float	GetFontSize();

	// 构建字体
	bool			Attach(const char* lpsTTFile, float nFontSize);

private:

	// 根据字符获取轮廓
	unsigned int			GetGlyphByCharacter( wchar_t c );
	inline float			GetWidthFromCharacter( wchar_t c );
	inline void				CacheCharacter(unsigned int idx);


	FT_Library				m_FTLibrary;
	FT_Face					m_FTFace;
	unsigned char*			m_pFontBuffer;
	HGE*					m_pHGE;
	hgeSprite*				m_pSprite;

	std::vector<TTFONTGLYPH>		m_Glyphs;

	float					m_nKerningWidth;
	float					m_nKerningHeight;
	float					m_nFontSize;

};//class TypeFont


#endif //TYPEFONT_H
